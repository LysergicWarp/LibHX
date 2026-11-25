#include <stdint.h>

#include <hx/test.h>
#include <hx/text.h>
#include <hx/vdp2.h>

#include "font.h"

/* The text calls with a font of the program's own, one of its sources, on NBG0 with pattern
 * names of two words. The picture is the run's screenshot to show. */

/* VRAM, half A: an empty cell, the font's four glyphs as cells of colour 5 on nothing, then
 * as cells of colour 6 on colour 7, and the page of pattern names. The back screen's colour is
 * half B's last word */
#define EMPTY_CELL 0x00000
#define CELLS 0x00020
#define BOXED_CELLS 0x000A0
#define NAMES 0x04000
#define BACK 0x7FFFE

/* A pattern name of two words: a palette in the first, a character number in the second,
 * which counts 32 bytes of VRAM */
#define PALETTE 2
#define NAME(cells) ((uint32_t)((PALETTE << 16) | ((cells) >> 5)))

#define BACK_COLOR VDP2_RGB(8, 8, 8)

static const vdp2_scroll_t format = {
        .nbg = {
                {
                        .colors = VDP2_COLORS_16,
                        .character = VDP2_CHARACTER_8,
                        .names = VDP2_NAMES_2_WORDS,
                        .plane = VDP2_PLANE_1X1,
                        .planes = { NAMES, NAMES, NAMES, NAMES }
                }
        }
};

/* A word of the pattern name at a column and row, as the chip has it */
static uint32_t
name_word(uint32_t column, uint32_t row, uint32_t word)
{
        return *(volatile uint16_t *)(vdp2_vram(NAMES) + (((row * 64) + column) << 1) + word);
}

int
main(void)
{
        uint16_t * const empty = vdp2_vram(EMPTY_CELL);
        uint16_t * const names = vdp2_vram(NAMES);
        const hx_text_t text = {
                .font = &test_font,
                .page = names,
                .names = VDP2_NAMES_2_WORDS,
                .first = NAME(CELLS)
        };
        const hx_text_t boxed = {
                .font = &test_font,
                .page = names,
                .names = VDP2_NAMES_2_WORDS,
                .first = NAME(BOXED_CELLS)
        };

        vdp2_start();

        *vdp2_cram((PALETTE << 4) + 5) = VDP2_RGB(31, 0, 0);
        *vdp2_cram((PALETTE << 4) + 6) = VDP2_RGB(31, 31, 31);
        *vdp2_cram((PALETTE << 4) + 7) = VDP2_RGB(0, 0, 31);

        /* The font has no space: the page is all the empty cell, by a name of 0 */
        for (uint32_t word = 0; word < 16; word++) {
                empty[word] = 0;
        }
        for (uint32_t word = 0; word < 64 * 64 * 2; word++) {
                names[word] = 0;
        }

        hx_test_equal("cells", hx_text_cells(&test_font, vdp2_vram(CELLS), 5, 0), 4);
        hx_test_equal("boxed", hx_text_cells(&test_font, vdp2_vram(BOXED_CELLS), 6, 7), 4);

        hx_test_equal("string", hx_text_string(&text, 1, 1, "ABCD"), 4);
        hx_test_equal("name.palette", name_word(1, 1, 0), PALETTE);
        hx_test_equal("name.first", name_word(1, 1, 1), CELLS >> 5);
        hx_test_equal("name.last", name_word(4, 1, 1), (CELLS >> 5) + 3);
        hx_test_equal("name.past", name_word(5, 1, 1), 0);

        /* A code above the font's glyphs and one below them are both the frame */
        hx_test_equal("fallback", hx_text_string(&text, 1, 3, "Z A"), 3);
        hx_test_equal("fallback.above", name_word(1, 3, 1), (CELLS >> 5) + 3);
        hx_test_equal("fallback.below", name_word(2, 3, 1), (CELLS >> 5) + 3);

        /* Digits the font has glyphs for */
        hx_test_equal("hex", hx_text_hex(&text, 1, 5, 0xDCBA, 4), 4);
        hx_test_equal("hex.first", name_word(1, 5, 1), (CELLS >> 5) + 3);

        hx_test_equal("boxed.string", hx_text_string(&boxed, 1, 7, "CD"), 2);
        hx_test_equal("boxed.name", name_word(1, 7, 1), (BOXED_CELLS >> 5) + 2);

        vdp2_scroll_format(&format);
        /* NBG0's two reads, both from half A */
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_nbg_priority(1, 0, 0, 0);
        vdp2_show(VDP2_NBG0);

        vdp2_back_color(BACK, BACK_COLOR);
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        hx_test_end();
        for (;;) {
        }
}
