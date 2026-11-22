# Building a disc

`build/disc.mk` builds a disc image around a program: an ISO with the program as its first
file, a silent audio track and a CUE.

> [!NOTE]
> A disc build needs your own BIOS. The build cuts Sega's Security Code and the Area Code blocks
> from it, and the SDK holds no copy. The places are those of BIOS v1.01a: a file with other
> bytes there stops the build.

## The Makefile

The Makefile of [Your first program](first-program.md), with the disc added:

```make
HX_OUT:= build
HX_PROGRAM:= $(HX_OUT)/game
HX_SOURCES:= main.c
HX_PROGRAM_CFLAGS:= -std=c23 -O2

HX_DISC:= $(HX_OUT)/game
HX_DISC_FIRST:= $(HX_PROGRAM).bin
HX_IP_TITLE:= MY GAME
HX_IP_DATE:= 20261121

all:

include $(HX_ROOT)/build/program.mk
include $(HX_ROOT)/build/disc.mk

all: $(HX_DISC_IMAGE)
```

- `HX_DISC` is the image's path without an extension.
- `HX_DISC_FIRST` is the file the BIOS loads: the program.
- `HX_IP_TITLE` and `HX_IP_DATE` are the Game Title and the Release Date of the disc's header.
- `HX_DISC_IMAGE` names the image's files, for a goal to depend on.

## Build

In `game`, with the BIOS file named:

```
pwsh ../LibHX/tools/hx-make.ps1 -Bios ../bios.bin
```

`HX_BIOS` on the host names the BIOS as well, in place of `-Bios`.

| File | What |
| --- | --- |
| `build/game.cue` | The image: the file an emulator or a burner takes |
| `build/game.iso` | The data track |
| `build/game.silence.bin` | The audio track |

## More detail

- [Disc settings](../reference/disc.md): the other fields of the header, files beside the
  program, the volume's name.
- [The disc image](../in-depth/disc.md): the tracks, the Initial Program, the Security Code.
