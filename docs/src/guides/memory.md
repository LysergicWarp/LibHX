# Startup and memory

libhxrt's startup runs between the BIOS and `main`. A program is linked into Work RAM-H, and
single objects go to Work RAM-L by name.

## What main finds

- The stack pointer at the stack's end.
- The cache purged and on.
- Every object without an initial value zeroed, and every other holding its own.
- The vector table, GBR and the interrupt masks as the BIOS left them.

`main` takes no arguments. If it returns, startup stops in a loop.

> [!NOTE]
> No constructor or destructor of a static object is run yet. A program with one does not link.

## Work RAM-L

`<hx/start.h>` places an object in Work RAM-L:

```c
#include <stdint.h>

#include <hx/start.h>

HX_LOW_RAM static uint8_t buffer[0x40000];
HX_LOW_RAM_DATA static uint16_t steps[] = { 1, 2, 4 };
```

- `HX_LOW_RAM` is for an object without an initial value. It is zeroed, and takes no room in
  the file the BIOS loads.
- `HX_LOW_RAM_DATA` is for one with. Its value is in the file, and startup copies it over.

## The stack

The program's Makefile picks where the stack is:

```make
HX_STACK:= low
```

| `HX_STACK` | Where | Size |
| --- | --- | --- |
| `high` | The top of Work RAM-H | `HX_STACK_SIZE`, 16 kbytes when it is not set |
| `low` | Below the program | 12 kbytes at the usual load address |

- The stack at the top comes off the memory the program has in Work RAM-H.
- The one below the program costs it nothing.
- `high` is taken when `HX_STACK` is not set.

`hx_stack_start` and `hx_stack_end` give the stack's bounds:

```c
const uintptr_t size = (uintptr_t)hx_stack_end - (uintptr_t)hx_stack_start;
```

## More detail

- API: [`<hx/start.h>`](../api/start_8h.html).
- [Switches](../reference/switches.md): `HX_STACK` and `HX_STACK_SIZE`.
- [Build settings](../reference/build.md#programmk): the load address.
- [Startup and the linker script](../in-depth/startup.md): each step of startup, where every
  section goes, what lies under the low stack.
