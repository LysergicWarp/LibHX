# VDP2

`<hx/vdp2.h>` is lib315's driver for VDP2. It starts the chip, sets the screen mode, colours
the back screen, and shows the four normal scroll screens as tiles. The back screen is the
one behind every other, which shows where nothing else does.

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

- `vdp2_start` puts the chip's registers to stated values, nearly all the ones a reset
  leaves. The display is off and the screen black. It comes first: no other call counts on
  what the BIOS left in the chip.
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

## Tiles on screen

A normal scroll screen, NBG0 to NBG3, is a map of tiles. This one is a yellow checkerboard
over a blue back screen:

```c
#include <stdint.h>

#include <hx/vdp2.h>

/* VRAM: two cells, a page of pattern names, the back screen's colour */
#define CELLS 0x00000
#define NAMES 0x02000
#define BACK 0x7FFFE

static const vdp2_scroll_t format = {
        .nbg = {
                {
                        .colors = VDP2_COLORS_16,
                        .character = VDP2_CHARACTER_8,
                        .names = VDP2_NAMES_1_WORD_FLIP,
                        .plane = VDP2_PLANE_1X1,
                        .planes = { NAMES, NAMES, NAMES, NAMES }
                }
        }
};

int
main(void)
{
        uint16_t * const cells = vdp2_vram(CELLS);
        uint16_t * const names = vdp2_vram(NAMES);

        vdp2_start();

        /* Cell 0 is empty. Cell 1 is all colour 1, yellow */
        *vdp2_cram(1) = VDP2_RGB(31, 31, 0);
        for (uint32_t word = 0; word < 16; word++) {
                cells[word] = 0x0000;
                cells[16 + word] = 0x1111;
        }

        /* A page is 64 by 64 names: cell 1 on every other square */
        for (uint32_t at = 0; at < 64 * 64; at++) {
                names[at] = (at ^ (at >> 6)) & 1;
        }

        vdp2_scroll_format(&format);
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_nbg_priority(1, 0, 0, 0);
        vdp2_show(VDP2_NBG0);

        vdp2_back_color(BACK, VDP2_RGB(0, 0, 31));
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        for (;;) {
        }
}
```

The program writes three things:

- **Cells** in VRAM: a tile of 8 by 8 dots. With 16 colours a dot is 4 bits, a line two
  words and a cell 32 bytes. A dot of 0 is transparent.
- **Pattern names** in VRAM: one for each square of the map, which names the cell there by
  its place in VRAM, in steps of 32 bytes. A page of them is 64 by 64 cells.
- **Colours** in the colour RAM, through `vdp2_cram`. A dot is the number of one.

Four calls then show the screen:

- `vdp2_scroll_format` says what the four screens are made of: the colours of a dot, the
  size of a pattern name, where the pages of names are. It takes all four screens at once,
  and one that does not show is left at 0.
- `vdp2_cycle` gives a screen its reads of VRAM. A screen reads its pattern names and its
  cells from the bank they are in, each in a timing of the bank's cycle. Without them it does
  not show as it should.
- `vdp2_nbg_priority` puts the screens in order. A screen of priority 0 does not show.
- `vdp2_show` names the screens that show.

## VRAM is the program's

The driver takes no VRAM of its own. A table's place is an offset into VRAM in bytes, and the
program decides it.

- `vdp2_vram` gives the word at an offset.
- An offset wraps at the VRAM's size, 512 kbytes, and its bit 0 is dropped.
- VRAM is two halves of 256 kbytes. After `vdp2_start` the CPU reads and writes both at any
  time. A half that `vdp2_cycle` gives to a screen is the CPU's while the screen shows only
  in a timing the cycle leaves it: `VDP2_ACCESS_CPU`.

## More detail

- API: [`<hx/vdp2.h>`](../api/lib315_2include_2hx_2vdp2_8h.html).
- [Register maps](registers.md): `<hx/hw/315/vdp2.h>`, the chip's registers and memory.
