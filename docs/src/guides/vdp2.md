# VDP2

`<hx/vdp2.h>` is lib315's driver for VDP2. It starts the chip, sets the screen mode and colours
the back screen: the screen behind every other, which shows where nothing else does.

## A colour on screen

```c
#include <hx/vdp2.h>

int
main(void)
{
        vdp2_start();
        vdp2_back_color(0x7FFFE, VDP2_RGB(0, 0, 31));
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        for (;;) {
        }
}
```

- `vdp2_start` puts every register to the value a reset leaves. The display is off and the
  screen black. It comes first: no other call counts on what the BIOS left in the chip.
- `vdp2_back_color` writes the colour to VRAM, at the offset the program names, and puts the
  back screen's table there.
- `vdp2_screen` sets the screen mode and turns the display on.
- `VDP2_RGB` makes a colour of red, green and blue, each 0 to 31.

## The screen mode

`vdp2_screen` takes the four fields of the chip's TV mode register:

| Value | Choices |
| --- | --- |
| Width | `VDP2_WIDTH_320`, `_352`, `_640`, `_704`, and four for a 31 kHz or a Hi-Vision monitor |
| Lines | `VDP2_HEIGHT_224`, `_240`, `_256` |
| Interlace | `VDP2_INTERLACE_NONE`, `_SINGLE`, `_DOUBLE` |
| Border | `VDP2_BORDER_BLACK`, `VDP2_BORDER_BACK` for the back screen's colour |

- 352 and 704 pixels run on the system clock's other setting, which the boot ROM's clock
  change routine sets. `vdp2_screen` leaves the clock as it is.
- A clock change puts VDP2 back as a reset leaves it and does not keep VRAM, so `vdp2_start`
  and the tables come after it.
- 256 lines is for a PAL console alone. `vdp2_pal` says which the console is.
- Called with the display on, it changes the mode at the start of the next vertical blank.

## A colour for each line

```c
#include <stdint.h>

#include <hx/vdp2.h>

/* The table's place: the last 512 bytes of VRAM */
#define BACK_TABLE 0x7FE00

int
main(void)
{
        uint16_t * const table = vdp2_vram(BACK_TABLE);

        vdp2_start();
        for (uint32_t line = 0; line < 224; line++) {
                table[line] = VDP2_RGB(0, 0, line >> 3);
        }
        vdp2_back_lines(BACK_TABLE);
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        for (;;) {
        }
}
```

- The table is a word a line, from the top. The program fills it.
- With single interlace a word is two lines.
- A border in the back screen's colour takes the colour of the picture's last line.

## VRAM is the program's

The driver takes no VRAM of its own. A table's place is an offset into VRAM in bytes, and the
program decides it.

- `vdp2_vram` gives the word at an offset.
- An offset wraps at the VRAM's size, 512 kbytes, and its bit 0 is dropped.
- After `vdp2_start`, and while no scroll screen is on, the CPU reads and writes VRAM at any
  time.

## More detail

- API: [`<hx/vdp2.h>`](../api/lib315_2include_2hx_2vdp2_8h.html).
- [Register maps](registers.md): `<hx/hw/315/vdp2.h>`, the chip's registers and memory.
