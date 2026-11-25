#include <stddef.h>
#include <stdint.h>

#include <hx/font/unscii8.h>
#include <hx/test.h>
#include <hx/text.h>
#include <hx/vdp2.h>

/* The text calls with the primary font on NBG0: every glyph, a string, numbers, a code the
 * font does not hold, and places that wrap at the page's edges. The picture is the run's
 * screenshot to show. */

/* VRAM, half A: the glyphs as cells of white on nothing, then as cells of colour 2 on colour
 * 3, a place for cells that do not show, and the page of pattern names. The back screen's
 * colour is half B's last word */
#define CELLS 0x00000
#define BOXED_CELLS 0x00C00
#define SCRATCH_CELLS 0x01800
#define NAMES 0x04000
#define BACK 0x7FFFE

/* A pattern name of one word: a palette in the top 4 bits, then a character number, which
 * counts 32 bytes of VRAM */
#define NAME(palette, cells) ((uint32_t)(((palette) << 12) | ((cells) >> 5)))

/* A glyph's count from the font's first */
#define GLYPH(code) ((uint32_t)(code) - 0x20)

#define BACK_COLOR VDP2_RGB(8, 8, 8)

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

/* The pattern name at a column and row, as the chip has it */
static uint32_t
name(uint32_t column, uint32_t row)
{
        return *(volatile uint16_t *)(vdp2_vram(NAMES) + (row * 64) + column);
}

/* A word of a set of cells, as the chip has it */
static uint32_t
cell_word(uint32_t cells, uint32_t word)
{
        return *(volatile uint16_t *)(vdp2_vram(cells) + word);
}

int
main(void)
{
        /* White, by the primary font as a null pointer */
        const hx_text_t plain = {
                .page = vdp2_vram(NAMES),
                .names = VDP2_NAMES_1_WORD_FLIP,
                .first = NAME(0, CELLS)
        };
        /* The same cells with palette 1: green */
        const hx_text_t tinted = {
                .page = vdp2_vram(NAMES),
                .names = VDP2_NAMES_1_WORD_FLIP,
                .first = NAME(1, CELLS)
        };
        /* Yellow on blue, by the font's own name */
        const hx_text_t boxed = {
                .font = &hx_font_unscii8,
                .page = vdp2_vram(NAMES),
                .names = VDP2_NAMES_1_WORD_FLIP,
                .first = NAME(1, BOXED_CELLS)
        };
        /* Two fonts the tile calls make no cell of */
        hx_font_t tall = hx_font_unscii8;
        hx_font_t wide = hx_font_unscii8;

        tall.height = 16;
        wide.width = 16;

        hx_test_equal("primary", (uint32_t)(uintptr_t)hx_font,
            (uint32_t)(uintptr_t)&hx_font_unscii8);

        vdp2_start();

        *vdp2_cram(1) = VDP2_RGB(31, 31, 31);
        *vdp2_cram(16 + 1) = VDP2_RGB(0, 31, 0);
        *vdp2_cram(16 + 2) = VDP2_RGB(31, 31, 0);
        *vdp2_cram(16 + 3) = VDP2_RGB(0, 0, 31);

        /* The cells. The second glyph is !, whose top row is 0x18: two dots in the middle */
        hx_test_equal("cells", hx_text_cells(NULL, vdp2_vram(CELLS), 1, 0), 96);
        hx_test_equal("cells.left", cell_word(CELLS, 16), 0x0001);
        hx_test_equal("cells.right", cell_word(CELLS, 17), 0x1000);
        hx_test_equal("cells.last", cell_word(CELLS, (95 * 16) + 2), 0x0111);
        hx_test_equal("boxed", hx_text_cells(&hx_font_unscii8, vdp2_vram(BOXED_CELLS), 2, 3), 96);
        hx_test_equal("boxed.left", cell_word(BOXED_CELLS, 16), 0x3332);
        hx_test_equal("boxed.right", cell_word(BOXED_CELLS, 17), 0x2333);

        /* A font that is not 8 by 8 writes nothing */
        *vdp2_vram(SCRATCH_CELLS) = 0x5A5A;
        hx_test_equal("cells.tall", hx_text_cells(&tall, vdp2_vram(SCRATCH_CELLS), 1, 0), 0);
        hx_test_equal("cells.tall.word", cell_word(SCRATCH_CELLS, 0), 0x5A5A);
        hx_test_equal("cells.wide", hx_text_cells(&wide, vdp2_vram(SCRATCH_CELLS), 1, 0), 0);
        hx_test_equal("cells.wide.word", cell_word(SCRATCH_CELLS, 0), 0x5A5A);

        /* Ink and paper are cut to 4 bits: uncut, the word would be 0x4421 */
        hx_test_equal("cells.cut", hx_text_cells(hx_font, vdp2_vram(SCRATCH_CELLS), 0x21, 0x40), 96);
        hx_test_equal("cells.cut.word", cell_word(SCRATCH_CELLS, 16), 0x0001);

        /* The whole page as spaces, its last name too: a count past the page's names is cut
         * to them */
        vdp2_vram(NAMES)[(63 * 64) + 63] = 0x5A5A;
        hx_test_equal("fill.page", hx_text_fill(&plain, 0, 0, ' ', 5000), 4096);
        hx_test_equal("fill.page.last", name(63, 63), GLYPH(' '));

        /* Every glyph, 32 a row */
        hx_test_equal("string", hx_text_string(&plain, 4, 1,
            " !\"#$%&'()*+,-./0123456789:;<=>?"), 32);
        hx_text_string(&plain, 4, 2, "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
        hx_text_string(&plain, 4, 3, "`abcdefghijklmnopqrstuvwxyz{|}~\x7F");
        hx_test_equal("string.second", name(5, 1), GLYPH('!'));
        hx_test_equal("string.last", name(35, 3), GLYPH(0x7F));

        hx_test_equal("boxed.string", hx_text_string(&boxed, 4, 5, "hx text: Unscii 8x8"), 19);
        hx_test_equal("boxed.name", name(4, 5), NAME(1, BOXED_CELLS) + GLYPH('h'));

        hx_test_equal("hex", hx_text_hex(&plain, 4, 7, 0x0123CDEF, 8), 8);
        hx_test_equal("hex.first", name(4, 7), GLYPH('0'));
        hx_test_equal("hex.last", name(11, 7), GLYPH('F'));
        hx_test_equal("hex.low", hx_text_hex(&plain, 14, 7, 0x89AB4567, 4), 4);
        hx_test_equal("hex.low.first", name(14, 7), GLYPH('4'));
        hx_test_equal("hex.cut", hx_text_hex(&tinted, 20, 7, 0xFEDCBA98, 9), 8);
        hx_test_equal("hex.cut.first", name(20, 7), NAME(1, CELLS) + GLYPH('F'));
        hx_test_equal("hex.cut.past", name(28, 7), GLYPH(' '));
        hx_test_equal("hex.none", hx_text_hex(&plain, 30, 7, 0xFFFFFFFF, 0), 0);
        hx_test_equal("hex.none.name", name(30, 7), GLYPH(' '));

        hx_test_equal("decimal.zero", hx_text_decimal(&plain, 4, 9, 0), 1);
        hx_test_equal("decimal.zero.name", name(4, 9), GLYPH('0'));
        hx_test_equal("decimal.most", hx_text_decimal(&plain, 8, 9, 4294967295), 10);
        hx_test_equal("decimal.most.first", name(8, 9), GLYPH('4'));
        hx_test_equal("decimal.most.last", name(17, 9), GLYPH('5'));
        hx_test_equal("decimal.tinted", hx_text_decimal(&tinted, 20, 9, 1000), 4);
        hx_test_equal("decimal.three", hx_text_decimal(&plain, 26, 9, 907), 3);

        /* Codes below the font's first glyph and above its last show the fallback glyph */
        hx_test_equal("fallback", hx_text_string(&plain, 4, 11, "\x01\x1F\x80\xFF!"), 5);
        hx_test_equal("fallback.below", name(5, 11), GLYPH(0x7F));
        hx_test_equal("fallback.above", name(6, 11), GLYPH(0x7F));

        /* A run, spaces over a part of it, and a code of more than 8 bits */
        hx_test_equal("fill", hx_text_fill(&boxed, 4, 13, '-', 10), 10);
        hx_test_equal("fill.over", hx_text_fill(&plain, 7, 13, ' ', 2), 2);
        hx_test_equal("fill.code", hx_text_fill(&plain, 16, 13, 0x100 + '=', 3), 3);
        hx_test_equal("fill.code.name", name(16, 13), GLYPH('='));
        hx_test_equal("fill.none", hx_text_fill(&plain, 20, 13, '=', 0), 0);
        hx_test_equal("fill.none.name", name(20, 13), GLYPH(' '));

        /* Past column 63 comes the row below, whether the string runs there or starts
         * there. A row past 63 is a row from the top again, and the page's last name is
         * followed by its first */
        hx_text_string(&plain, 62, 15, "..AB");
        hx_test_equal("wrap.row", name(0, 16), GLYPH('A'));
        hx_text_string(&plain, 64 + 5, 17, "C");
        hx_test_equal("wrap.column", name(5, 18), GLYPH('C'));
        hx_text_string(&plain, 2, 64 + 20, "D");
        hx_test_equal("wrap.rows", name(2, 20), GLYPH('D'));
        hx_text_string(&plain, 63, 63, "EF");
        hx_test_equal("wrap.page", name(0, 0), GLYPH('F'));

        /* The screen's other three corners */
        hx_text_fill(&tinted, 39, 0, '+', 1);
        hx_text_fill(&tinted, 0, 27, '+', 1);
        hx_text_fill(&tinted, 39, 27, '+', 1);

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
