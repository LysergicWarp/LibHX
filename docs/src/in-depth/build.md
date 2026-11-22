# The build

How a program's build is put together, past what
[Your first program](../start/first-program.md) needs.

## The libraries build with the program

A program builds the SDK's libraries from the clone into its own build folder, `HX_OUT`. Nothing
is written beside the sources, and the clone is left as it is.

Each library has a make of its own, which decides what is out of date. It runs on every build
of the program.

## What the SDK fixes

Every object, the SDK's and the program's, is compiled with these:

| Flag | Why |
| --- | --- |
| `-m2 -mb` | The SH-2, big-endian |
| `-ffreestanding` | No C library |
| `-ffunction-sections -fdata-sections` | A section for each function and object, so a link drops what a program does not use |
| `-fno-delete-null-pointer-checks` | Address 0 is the BIOS ROM and reads without a fault, so an access through a pointer does not prove it non-null |

The calling convention is GCC's default.

A program's C++ is built without exceptions and run-time type information. The first use of a
local static object takes no lock, and a destructor of a static object is not handed to a C++
runtime.

The program's own flags come first on the command line, so none of them undoes what the SDK
fixes.

## The SDK's own sources

The libraries are C23 at `-O2`, whatever the program's level, with these warnings as errors:

```
-Wall -Wextra -Wshadow -Wundef -Wvla -Wdouble-promotion -Werror
-Wstrict-prototypes -Wmissing-prototypes
```

A program takes them or leaves them. `HX_WARNINGS` names the first line and `HX_C_WARNINGS` the
second, which is for C alone. They come with `program.mk`, so a setting that uses them is given
with `=`:

```make
HX_PROGRAM_CFLAGS= -std=c23 -O2 $(HX_WARNINGS) $(HX_C_WARNINGS)
```

## The link

- The linker script places every section. One it does not name stops the link.
- What nothing reaches from startup is dropped.
- libgcc is the compiler's own, for what the compiler calls.
- The linker's warnings stop the link.

## Settings are symbols

A choice the SDK leaves to the program is a setting in its Makefile. It reaches the link as a
symbol, so the libraries are the same archives whatever a program picks. `HX_STACK`,
`HX_STACK_SIZE` and `HX_IP_FIRST_READ` are the ones so far.

[Switches](../reference/switches.md) lists what a program may toggle.

## What is built anew

- A changed flag builds every object of the program anew.
- A changed setting builds what it goes into.
- A change to a makefile does both.

A value given on make's command line counts as a change.

## The same bytes anywhere

The same sources and toolchain give the same archives, program and disc image, byte for byte,
wherever the clone and the build folder are.

- Debug data names the program's folder as `.` and the clone as `LibHX`.
- An archive holds no time, owner or mode of its members.
- Every date on a disc is its Release Date.

## The toolchain image

`tools/image` holds the image's Dockerfile:

- binutils 2.47 and GCC 16.2.0 for the SH-2, as `sh2eb-elf-`
- GNU make
- xorriso

The image holds nothing of the SDK. Every input is pinned: the base image by digest, its
packages by a snapshot's date, the sources by checksum.

## Another runtime

A program that does not use `program.mk` includes `build/libs.mk` alone, for the include paths
and the archives. [Build settings](../reference/build.md#libsmk) lists what it gives.
