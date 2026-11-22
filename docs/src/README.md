# LibHX

hx is a Sega Saturn SDK in C, with SH-2 assembly on its hot paths. It is in early development.
The math library, the runtime's startup with the RAM cartridge, the C library's memory
functions, and VDP2's start with its back screen are the code so far.

## Libraries

Each library depends only on those below it. `libhx2d` and `libhxio` sit side by side, as do
`libhxrt` and `libhxmath`: neither of a pair depends on the other.

| Library | Contents | State |
| --- | --- | --- |
| `libhx3d` | 3D pipeline | Planned |
| `libhx2d` | 2D helpers: sprites, tile maps, fonts | Planned |
| `libhxio` | Files on disc, loading and decompression | Planned |
| [`lib315`](guides/vdp2.md) | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card | In progress |
| [`libhxrt`](guides/memory.md) | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library | In progress |
| [`libhxmath`](guides/math.md) | Fixed point, trig, vectors, matrices | In progress |

The [register maps](guides/registers.md) the libraries share are headers with no archive, under
`hw/`.

## How these pages are laid out

- Getting started: from a clone to a disc image, in three pages. Begin with
  [Setting up](start/setup.md).
- Guides: one topic a page, code first.
- Reference: the [switches](reference/switches.md) a program may toggle, every setting and
  option, and the [API](reference/api.md).
- In depth: how the SDK works underneath. No guide needs it read first.

A guide ends with links to its reference and in-depth pages.
