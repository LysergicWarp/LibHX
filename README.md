# LibHX

hx is a Sega Saturn SDK. It is in early development and has no usable code yet.

Planned libraries, each depending only on those below it:

| Library | Contents |
| --- | --- |
| `libhx3d` | 3D pipeline |
| `libhx` | Math, files on disc, loading and decompression, 2D helpers |
| `lib315` | Drivers for Sega's chips: VDP1, VDP2, SCU, SMPC, sound, CD block, MPEG card |
| `libhxrt` | Runtime: startup, interrupts, DMA, memory, the second SH-2, the C library |

Hot paths are SH-2 assembly behind a C API.
