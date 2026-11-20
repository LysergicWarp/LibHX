# LibHX

hx is a Sega Saturn SDK. It is in early development. The math library and the runtime's
startup are the code so far.

Its libraries, each depending only on those below it. `libhxrt` and `libhxmath` sit side by
side: neither depends on the other.

| Library | Contents | State |
| --- | --- | --- |
| `libhx3d` | 3D pipeline | Planned |
| `libhx` | Files on disc, loading and decompression, 2D helpers | Planned |
| `lib315` | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card | Planned |
| [`libhxrt`](libhxrt/README.md) | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library | In progress |
| [`libhxmath`](libhxmath/README.md) | Fixed point, trig, vectors, matrices | In progress |

Hot paths are SH-2 assembly behind a C API.

[`hw/`](hw) holds the register maps the libraries share, as headers with no archive:
`<hx/hw/sh2/...>` for the SH-2's own modules and `<hx/hw/315/...>` for Sega's chips. A map gives
each register's address as a number, for C and assembly alike, a struct over the module for C,
and the names of the bits. So far: the SH-2's cache, free-running timer and divide unit, the
SCU, and VDP2's registers without their bits.

## Build

A program builds the SDK's libraries from the clone into its own build folder. Its Makefile sets
`HX_ROOT` to the clone and `HX_OUT` to a folder of its build, names its sources, and includes
[`build/program.mk`](build/program.mk). That compiles the program, links it against the
libraries with what it does not use left out, and gives the file the BIOS loads. A program on
another runtime includes [`build/libs.mk`](build/libs.mk) alone, for the include paths and the
archives.

A choice the SDK leaves to the program is a setting in its Makefile. It reaches the link as a
symbol, so the libraries are the same archives whatever a program picks. So far: the stack at
the top of Work RAM-H or below the program, and its size.

The libraries build with the flags in [`build/flags.mk`](build/flags.mk) and
[`build/fixed.mk`](build/fixed.mk): C23 at `-O2`, GCC's default calling convention, warnings as
errors. `make headers` in a library's folder checks each
of its public headers alone, as C11, C23 and C++23.

[`build/disc.mk`](build/disc.mk) builds a disc image around a program: the Initial Program, an
ISO with the program as its first file, a silent audio track and a CUE. A Saturn disc has one
data track and one audio track at least, each of four seconds or more. The program's Makefile
sets the image's path, the title, the release date and the program's file, and may set the
other fields of the System ID. The same files give the same image, byte for byte.

Every disc carries Sega's Security Code and an Area Code block for each area it runs in. The
build cuts both from your own BIOS, named by `HX_BIOS`. The SDK holds no copy. The places are
those of BIOS v1.01a, and a file with other bytes there stops the build.

[`tools/image`](tools/image/Dockerfile) holds the toolchain image: binutils and GCC for the SH-2,
GNU make and xorriso. [`tools/hx-make.ps1`](tools/hx-make.ps1) runs make in it.

## Tests

A test is a disc: a program in `<library>/test/<name>` that leaves a report in work RAM, a row
for each thing it checks ([`libhxtest`](libhxtest/include/hx/test.h),
[`build/test.mk`](build/test.mk)). [`tools/hx-test.ps1`](tools/hx-test.ps1) builds each test from
nothing, runs it on Beetle Saturn and reads the report. It builds each twice, at two places, and
the archives, the program and the disc image have to be the same bytes.

The script runs the emulator through a program that drives a libretro core without a window.
That program is not part of this repository. The script's header says what it asks of one.

## Licence

MIT ([LICENSE](LICENSE)).
