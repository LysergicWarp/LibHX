# LibHX

hx is a Sega Saturn SDK in C, with SH-2 assembly on its hot paths. It is in early development.
The math library and the runtime's startup are the code so far.

## Libraries

Each library depends only on those below it. `libhx2d` and `libhxio` sit side by side, as do
`libhxrt` and `libhxmath`: neither of a pair depends on the other.

| Library | Contents | State |
| --- | --- | --- |
| `libhx3d` | 3D pipeline | Planned |
| `libhx2d` | 2D helpers: sprites, tile maps, fonts | Planned |
| `libhxio` | Files on disc, loading and decompression | Planned |
| `lib315` | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card | Planned |
| `libhxrt` | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library | In progress |
| `libhxmath` | Fixed point, trig, vectors, matrices | In progress |

The register maps the libraries share are headers with no archive, under `hw/`:
`<hx/hw/sh2/...>` for the SH-2's own modules and `<hx/hw/315/...>` for Sega's chips.

## Where to start

- [Setting up](guides/setup.md): the toolchain image, and the script that runs make in it.
- [API reference](reference.md): the public headers, generated from their comments.
