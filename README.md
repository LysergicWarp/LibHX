# LibHX

hx is a Sega Saturn SDK. It is in early development. The math library is the only one with
code so far.

Its libraries, each depending only on those below it. `libhxrt` and `libhxmath` sit side by
side: neither depends on the other.

| Library | Contents | State |
| --- | --- | --- |
| `libhx3d` | 3D pipeline | Planned |
| `libhx` | Files on disc, loading and decompression, 2D helpers | Planned |
| `lib315` | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card | Planned |
| `libhxrt` | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library | Planned |
| [`libhxmath`](libhxmath/README.md) | Fixed point, trig, vectors, matrices | In progress |

Hot paths are SH-2 assembly behind a C API.

## Build

A program builds the SDK's libraries from the clone into its own build folder. Its Makefile sets
`HX_ROOT` to the clone and `HX_OUT` to a folder of its build, and includes
[`build/libs.mk`](build/libs.mk). That gives it the include paths and the archives to link.

The libraries build with the flags in [`build/flags.mk`](build/flags.mk): C23 at `-O2`, GCC's
default calling convention, warnings as errors. `make headers` in a library's folder checks each
of its public headers alone, as C11, C23 and C++23.

[`tools/image`](tools/image/Dockerfile) holds the toolchain image: binutils and GCC for the SH-2,
GNU make and xorriso. [`tools/hx-make.ps1`](tools/hx-make.ps1) runs make in it.

## Licence

MIT ([LICENSE](LICENSE)).
