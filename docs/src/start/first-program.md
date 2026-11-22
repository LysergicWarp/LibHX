# Your first program

A program is a folder with a Makefile and its sources. The Makefile names the sources and
includes `build/program.mk`, which builds the SDK's libraries, the program, and the file the
BIOS loads.

## The source

`main.c`, in a folder `game` beside the clone:

```c
#include <hx/math.h>

int
main(void)
{
        hx_mat_t turn;

        hx_mat_rotation_y(HX_DEG(45), &turn);

        for (;;) {
        }
}
```

hx has no video driver yet, so this program shows nothing. It builds a matrix and waits.

## The Makefile

`Makefile`, beside it:

```make
HX_OUT:= build
HX_PROGRAM:= $(HX_OUT)/game
HX_SOURCES:= main.c
HX_PROGRAM_CFLAGS:= -std=c23 -O2

all: $(HX_PROGRAM).bin

include $(HX_ROOT)/build/program.mk
```

- `HX_OUT` is the folder of the build. The SDK's libraries are built there too.
- `HX_PROGRAM` is the program's path without an extension.
- `HX_SOURCES` names the sources: `.c`, `.cpp` and `.sx` (assembly).
- `HX_PROGRAM_CFLAGS` is what the program's C is compiled with.
- `HX_ROOT` is the clone. `hx-make.ps1` sets it.

## Build

In `game`:

```
pwsh ../LibHX/tools/hx-make.ps1
```

| File | What |
| --- | --- |
| `build/game.bin` | The file the BIOS loads |
| `build/game.elf` | The linked program, with its symbols |
| `build/game.map` | The link map |

Next: [Building a disc](disc.md).

## More detail

- [Switches](../reference/switches.md): what a program may toggle, the level of its own code
  among them.
- [Build settings](../reference/build.md): every setting of `program.mk`.
- [Startup and memory](../guides/memory.md): what `main` finds.
- [The build](../in-depth/build.md): the flags the SDK fixes, the link, what is built anew.
