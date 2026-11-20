# libhxrt

The runtime of hx: what stands between the BIOS and a program's `main`. Startup and the linker
script are the code so far.

## Startup

The BIOS loads the program to Work RAM-H and enters it at its first byte, which is startup's
([`src/start.sx`](src/start.sx)). Startup:

- sets the stack
- purges the SH-2's cache and turns it on, four ways, for instructions and data both
- zeroes every object without an initial value
- copies the initial values that belong outside Work RAM-H to their places
- calls `main`, and stops in a loop if it returns

The vector table, GBR and the interrupt masks stay as the BIOS left them. No constructor or
destructor of a static object is run yet, and a program with one does not link. Startup is
entered once: a second entry does not bring back the initial values in Work RAM-H.

## Memory

[`program.ld`](program.ld) links a program into Work RAM-H, from the address the BIOS loads it
to. [`<hx/start.h>`](include/hx/start.h) places single objects in Work RAM-L:

```c
HX_LOW_RAM static uint8_t buffer[0x40000];
HX_LOW_RAM_DATA static uint16_t steps[] = { 1, 2, 4 };
```

`HX_LOW_RAM` is for an object without an initial value, which takes no room in the file the
BIOS loads. `HX_LOW_RAM_DATA` is for one with.

## The stack

A program's Makefile picks where the stack is (`HX_STACK`, [`build/program.mk`](../build/program.mk)):

| `HX_STACK` | Where | Size |
| --- | --- | --- |
| `high` | The top of Work RAM-H | `HX_STACK_SIZE`, 16 kbytes when it is not set |
| `low` | Below the program, down to `0x06001000` | 12 kbytes at the usual load address |

The stack at the top comes off the memory the program has in Work RAM-H. The one below the
program costs it nothing: it lies over the Initial Program, which has run by then, and goes
down to the address the second SH-2's stack grows down from when the disc's header names
none. `hx_stack_start` and `hx_stack_end` give its bounds.

## Tests

`test/start` and `test/start-low-stack` check what `main` finds, one for each place of the
stack.
