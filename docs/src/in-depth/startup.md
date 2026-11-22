# Startup and the linker script

What happens between the BIOS and `main`, and where the link puts a program.
[Startup and memory](../guides/memory.md) is the short form.

## Startup

The BIOS loads the program to Work RAM-H and enters it at its first byte, which is startup's
(`libhxrt/src/start.sx`). Startup:

1. sets the stack
2. purges the SH-2's cache and turns it on, four ways, for instructions and data both
3. zeroes every object without an initial value
4. copies the initial values that belong in Work RAM-L to their places
5. calls `main`, and stops in a loop if it returns

The vector table, GBR and the interrupt masks of the SH-2 and the SCU stay as the BIOS left
them. The table is the BIOS's own, and the SH-2's mask is open.

Startup is entered once. The initial values in Work RAM-H are the loaded file's own bytes, so a
second entry would find them as the program left them.

## Where a program goes

`libhxrt/program.ld` links a program into Work RAM-H, from the address the BIOS loads it to:
`0x06004000` unless `HX_IP_FIRST_READ` says otherwise.

| What | Where | In the loaded file |
| --- | --- | --- |
| Code and constants | Work RAM-H | Yes |
| Objects with an initial value | Work RAM-H | Yes |
| Objects without one | Work RAM-H | No |
| `HX_LOW_RAM_DATA` objects | Work RAM-L | Yes, copied over by startup |
| `HX_LOW_RAM` objects | Work RAM-L | No |

Each line that places an object in Work RAM-L gets a section of its own, so the link drops the
ones nothing uses.

## The stack below the program

With `HX_STACK` as `low`, the stack runs from the program's first byte down to `0x06001000`.

- It lies over the Initial Program, which has run by then.
- It ends where the second SH-2's stack grows down from when the disc's header names none.
- Work RAM-H below that is the BIOS's own.

A load address above the usual one makes this stack larger.

## What stops the link

- A constructor or a destructor of a static object. Startup runs none yet.
- A section the linker script does not name.
- A stack size that leaves the program no memory, or one that is no multiple of 4.
- A load address that is not between the Initial Program and the end of Work RAM-H.
