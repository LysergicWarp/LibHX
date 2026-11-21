# LibHX

hx is a Sega Saturn SDK. It is in early development. The math library and the runtime's
startup are the code so far.

Its libraries, each depending only on those below it. `libhx2d` and `libhxio` sit side by
side, as do `libhxrt` and `libhxmath`: neither of a pair depends on the other.

| Library | Contents | State |
| --- | --- | --- |
| `libhx3d` | 3D pipeline | Planned |
| `libhx2d` | 2D helpers: sprites, tile maps, fonts | Planned |
| `libhxio` | Files on disc, loading and decompression | Planned |
| `lib315` | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card | Planned |
| [`libhxrt`](libhxrt/README.md) | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library | In progress |
| [`libhxmath`](libhxmath/README.md) | Fixed point, trig, vectors, matrices | In progress |

Hot paths are SH-2 assembly behind a C API.

[`hw/`](hw) holds the register maps the libraries share, as headers with no archive.

## Licence

MIT ([LICENSE](LICENSE)).
