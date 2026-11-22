#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* NBG0 in cell format: 16 colours, characters of one cell, pattern names of one word with the
 * two flips. The picture is the run's screenshot to show: three cells, two palettes, the four
 * flips, a transparent dot, and names at the screen's four corners. */

/* VRAM: the page of pattern names 8 kbytes into half A, the cells from the start of half B,
 * and the back screen's colour in half B's last word */
#define NAMES 0x02000
#define CELLS 0x40000
#define BACK 0x7FFFE

/* A page is 64 names across, and the screen shows 40 by 28 of them */
#define PAGE_WIDTH 64
#define COLUMNS 40
#define ROWS 28

/* A cell is 8 lines of two words, 4 bits a dot */
#define CELL_WORDS 16

/* The cells: nothing, one colour, and one with no symmetry */
#define EMPTY 0
#define SOLID 1
#define CORNER 2

/* A pattern name: a palette, the flips, and the low 10 bits of a character number. The number
 * counts 32 bytes of VRAM, and the bits above the 10 are the screen's supplement: the cells'
 * place */
#define NAME(palette, flips, cell) ((uint16_t)(((palette) << 12) | (flips) | (cell)))
#define SUPPLEMENT (CELLS >> 15)
#define FLIP_ACROSS 0x0400
#define FLIP_DOWN 0x0800

#define BACK_COLOR VDP2_RGB(8, 8, 8)

static const vdp2_scroll_t format = {
        .nbg = {
                {
                        .colors = VDP2_COLORS_16,
                        .character = VDP2_CHARACTER_8,
                        .names = VDP2_NAMES_1_WORD_FLIP,
                        .plane = VDP2_PLANE_1X1,
                        .planes = { NAMES, NAMES, NAMES, NAMES },
                        .supplement = SUPPLEMENT
                }
        }
};

static uint16_t *
name(uint32_t row, uint32_t column)
{
        return vdp2_vram(NAMES) + (row * PAGE_WIDTH) + column;
}

int
main(void)
{
        uint16_t * const cells = vdp2_vram(CELLS);

        hx_test_equal("cycle", VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG1_NAMES,
            VDP2_ACCESS_NBG2_NAMES, VDP2_ACCESS_NBG3_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
            VDP2_ACCESS_NBG0_CELL_SCROLL, VDP2_ACCESS_CPU, VDP2_ACCESS_NONE), 0x01234CEF);
        hx_test_equal("cycle.cut", VDP2_CYCLE(16 + 1, 16 + 2, 16 + 3, 16 + 4, 16 + 5, 16 + 6,
            16 + 7, 16 + 8), 0x12345678);
        hx_test_equal("cram", (uint32_t)(uintptr_t)vdp2_cram(3), VDP2_CRAM + 6);
        hx_test_equal("cram.wraps", (uint32_t)(uintptr_t)vdp2_cram(2048 + 3), VDP2_CRAM + 6);

        vdp2_start();

        /* Palette 0 is colours 0 to 15, palette 1 colours 16 to 31. Colour 0 of a palette is
         * the transparent dot's and does not show */
        *vdp2_cram(1) = VDP2_RGB(31, 0, 0);
        *vdp2_cram(2) = VDP2_RGB(0, 31, 0);
        *vdp2_cram(3) = VDP2_RGB(0, 0, 31);
        *vdp2_cram(16 + 1) = VDP2_RGB(31, 31, 31);
        *vdp2_cram(16 + 2) = VDP2_RGB(31, 31, 0);
        *vdp2_cram(16 + 3) = VDP2_RGB(0, 31, 31);
        /* Read as the chip has them, not as the compiler knows them. The colour RAM holds
         * 1024 colours, and the second 1024 words are the first */
        hx_test_equal("cram.word", *(volatile uint16_t *)vdp2_cram(16 + 2), VDP2_RGB(31, 31, 0));
        hx_test_equal("cram.twin", *(volatile uint16_t *)vdp2_cram(1024 + 3), VDP2_RGB(0, 0, 31));

        /* The corner: a top line of colour 1, a left column of colour 2, the rest colour 3,
         * and its last dot transparent */
        for (uint32_t word = 0; word < CELL_WORDS; word++) {
                cells[(EMPTY * CELL_WORDS) + word] = 0x0000;
                cells[(SOLID * CELL_WORDS) + word] = 0x1111;
                cells[(CORNER * CELL_WORDS) + word] = (word & 1) ? 0x3333 : 0x2333;
        }
        cells[(CORNER * CELL_WORDS) + 0] = 0x1111;
        cells[(CORNER * CELL_WORDS) + 1] = 0x1111;
        cells[(CORNER * CELL_WORDS) + CELL_WORDS - 1] = 0x3330;

        for (uint32_t at = 0; at < PAGE_WIDTH * PAGE_WIDTH; at++) {
                vdp2_vram(NAMES)[at] = NAME(0, 0, EMPTY);
        }
        *name(0, 0) = NAME(0, 0, SOLID);
        *name(0, 1) = NAME(0, 0, CORNER);
        *name(0, 2) = NAME(0, FLIP_ACROSS, CORNER);
        *name(0, 3) = NAME(0, FLIP_DOWN, CORNER);
        *name(0, 4) = NAME(0, FLIP_ACROSS | FLIP_DOWN, CORNER);
        *name(0, 5) = NAME(1, 0, CORNER);
        *name(0, COLUMNS - 1) = NAME(1, 0, SOLID);
        *name(1, 0) = NAME(1, 0, SOLID);
        *name(ROWS - 1, 0) = NAME(0, 0, SOLID);
        *name(ROWS - 1, COLUMNS - 1) = NAME(0, 0, CORNER);
        /* Past the screen's right and bottom edges, where nothing of them shows */
        *name(0, COLUMNS) = NAME(0, 0, SOLID);
        *name(ROWS, 0) = NAME(0, 0, SOLID);

        vdp2_scroll_format(&format);
        /* NBG0's two reads: its names from half A, its patterns from half B */
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_cycle(VDP2_BANK_B0, VDP2_CYCLE(VDP2_ACCESS_NONE, VDP2_ACCESS_NBG0_PATTERNS,
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
