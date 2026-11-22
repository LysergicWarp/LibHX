# Startup and memory

libhxrt's startup runs between the BIOS and `main`. A program is linked into Work RAM-H, and
single objects go to Work RAM-L by name. `<string.h>` copies and fills memory.

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

## Copying and filling

`<string.h>` has the C library's four memory functions:

```c
#include <stdint.h>
#include <string.h>

struct player {
        char name[8];
        uint32_t score;
};

static struct player players[4];
static struct player best;

int
main(void)
{
        memset(players, 0, sizeof players);
        memcpy(players[0].name, "SATURN", 7);
        players[0].score = 9000;

        memcpy(&best, &players[0], sizeof best);
        memmove(&players[1], &players[0], 3 * sizeof players[0]);

        if (memcmp(&best, &players[1], sizeof best) != 0) {
                return 1;
        }

        for (;;) {
        }
}
```

- `memcpy` copies between two blocks that do not overlap, and `memmove` between two that may.
- `memset` fills a block with one byte, and `memcmp` compares two blocks.
- They are for memory. A hardware register is written through its [map](registers.md).

> [!NOTE]
> The rest of the C library is not there yet.

## More detail

- API: [`<hx/start.h>`](../api/start_8h.html), and [`<string.h>`](../api/string_8h.html) for
  what the memory functions say past the C standard.
- [Switches](../reference/switches.md): `HX_STACK` and `HX_STACK_SIZE`.
- [Build settings](../reference/build.md#programmk): the load address.
- [Startup and the linker script](../in-depth/startup.md): each step of startup, where every
  section goes, what lies under the low stack.
- [The build](../in-depth/build.md): the calls of `memcpy` and `memset` the compiler makes on
  its own, and when a call by name is expanded in place.
