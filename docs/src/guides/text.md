# Text

`<hx/text.h>` is libhx2d's text on a scroll screen of VDP2. A font's glyphs become tiles, and
a character on screen is a pattern name.

## Text on screen

```c
#include <hx/text.h>
#include <hx/vdp2.h>

/* VRAM: the font's cells, a page of pattern names, the back screen's colour */
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
        const hx_text_t text = {
                .page = vdp2_vram(NAMES),
                .names = VDP2_NAMES_1_WORD_FLIP,
                .first = CELLS >> 5
        };

        vdp2_start();

        /* The glyphs as cells: colour 1, white, on nothing */
        *vdp2_cram(1) = VDP2_RGB(31, 31, 31);
        hx_text_cells(hx_font, vdp2_vram(CELLS), 1, 0);

        hx_text_fill(&text, 0, 0, ' ', 64 * 64);
        hx_text_string(&text, 2, 1, "Hello, Saturn");
        hx_text_hex(&text, 2, 3, 0x06004000, 8);
        hx_text_decimal(&text, 2, 5, 1994);

        vdp2_scroll_format(&format);
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_nbg_priority(1, 0, 0, 0);
        vdp2_show(VDP2_NBG0);

        vdp2_back_color(BACK, VDP2_RGB(0, 0, 16));
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        for (;;) {
        }
}
```

- `hx_text_cells` makes cells of a font's glyphs, once: 32 bytes of VRAM a glyph, at the
  place the program names. `hx_font` is the [primary font](#the-primary-font).
- `hx_text_t` says where text goes: the page of pattern names, the size of a name, and the
  name of the font's first glyph. `CELLS >> 5` is the first cell's character number.
- A pattern name of one word holds the low 10 bits of a character number, which reach the
  first 32 kbytes of VRAM. For cells further on, the bits above are the screen's
  `supplement` in `vdp2_scroll_format`, and `first` holds the low 10 alone.
- `hx_text_fill` writes one character many times. Here it clears the page with spaces.
- `hx_text_string` writes a string, `hx_text_hex` a count of hexadecimal digits,
  `hx_text_decimal` a number in decimal.
- The screen is set up with [`<hx/vdp2.h>`](vdp2.md#tiles-on-screen), as any tile screen is:
  16 colours and characters of 8 by 8 dots.

At 320 by 224 the screen shows 40 columns by 28 rows.

## Where a character goes

A call is told its column and row, and returns how many characters it wrote. There is no
cursor:

```c
uint32_t column = 2;

column += hx_text_string(&text, column, 7, "frame ");
column += hx_text_decimal(&text, column, 7, 1994);
```

- A page is 64 columns by 64 rows. After column 63 comes column 0 of the row below.
- A shorter number does not clear what a longer one left. `hx_text_fill` with a space does.
- A code the font has no glyph for shows the font's fallback glyph: a hollow square in the
  primary font.

## Colours

`hx_text_cells` takes two colours, each the number of a colour in the cell's palette: the
ink, for a glyph's dots, and the paper, for the others. A paper of 0 is transparent.

The palette is part of the pattern name. Text in another palette is another `hx_text_t` over
the same cells:

```c
const hx_text_t green = {
        .page = vdp2_vram(NAMES),
        .names = VDP2_NAMES_1_WORD_FLIP,
        .first = (1 << 12) | (CELLS >> 5)
};

*vdp2_cram(16 + 1) = VDP2_RGB(0, 31, 0);
hx_text_string(&green, 2, 9, "ready");
```

With pattern names of one word the palette is the name's top 4 bits. Palette 1 is colours 16
to 31.

## The primary font

`hx_font` is the primary font: one of the SDK's fonts, picked in the program's Makefile.

```make
HX_FONT:= unscii8
```

- `unscii8` is the one font so far, and the pick when the Makefile names none: 96 glyphs of
  8 by 8 dots for the codes 0x20 to 0x7F, from Viznut's
  [Unscii](https://viznut.fi/unscii/), which is in the public domain.
- A call takes the primary font where it is given none: a null pointer to `hx_text_cells`,
  or a `hx_text_t` with no `font`.
- A program does not define `hx_font` itself. The link stops on a second definition.
- A program that calls the text functions links the primary font, 784 bytes, whichever font
  it shows.

## A font of your own

A font is a `hx_font_t` in one of the program's sources:

```c
#include <hx/font.h>

extern const hx_font_t symbols;

/* Two glyphs, for the codes 0x80 and 0x81: a heart and a square */
static const uint8_t glyphs[2 * 8] = {
        0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00,
        0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00
};

const hx_font_t symbols = {
        .glyphs = glyphs,
        .count = 2,
        .first = 0x80,
        .fallback = 1,
        .width = 8,
        .height = 8
};
```

The program makes cells of it and names it in a `hx_text_t`:

```c
const hx_text_t marks = {
        .font = &symbols,
        .page = vdp2_vram(NAMES),
        .names = VDP2_NAMES_1_WORD_FLIP,
        .first = SYMBOL_CELLS >> 5
};

hx_text_cells(&symbols, vdp2_vram(SYMBOL_CELLS), 1, 0);
hx_text_string(&marks, 16, 1, "\x80\x81");
```

- A glyph is 8 bytes, a row each from the top. A byte's top bit is the left dot.
- `fallback` is the glyph shown for a code the font does not hold, counted from the first.
- The file goes in `HX_SOURCES`. It takes no setting, and any number of fonts sit beside the
  primary one.
- The text calls take fonts of 8 by 8 dots.

## Writing while the screen shows

The calls write VRAM. With the display on, a write to a bank a screen reads waits for a
timing the bank's cycle leaves the CPU: `VDP2_ACCESS_CPU` in `vdp2_cycle`.

The program at the top of this page writes its text ahead of `vdp2_screen`, and its cycle
leaves the CPU no timing. One that writes text later gives the bank one:

```c
vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
    VDP2_ACCESS_CPU, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
    VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
```

## More detail

- API: [`<hx/text.h>`](../api/text_8h.html), [`<hx/font.h>`](../api/font_8h.html), and the
  font's own header, [`<hx/font/unscii8.h>`](../api/unscii8_8h.html).
- [Switches](../reference/switches.md#hx_font): `HX_FONT`.
- [VDP2](vdp2.md): the tile screen the text is on.
- [The build](../in-depth/build.md#the-fonts): how the primary font reaches the link.
