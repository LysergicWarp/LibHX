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
