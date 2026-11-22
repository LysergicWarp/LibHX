#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* Two screens in cell format. NBG3 is 256 colours, characters of four cells and pattern names
 * of two words, its four planes each a page of their own, scrolled so the screen's middle is
 * where the four meet. NBG1 is 16 colours, characters of one cell and pattern names of one
 * word without flips, in front of it. The picture is the run's screenshot to show. */

/* VRAM, half A: NBG3's characters of 256 bytes each from its start, NBG1's one cell after
 * them, NBG1's page of pattern names 8 kbytes in */
#define CHARACTERS 0x00000
#define CELL 0x00600
#define CELL_NAMES 0x02000

/* VRAM, half B: NBG3's four pages of 4 kbytes each, which are pages 64 to 67 of its names,
 * and the back screen's colour in the last word */
#define PLANE_A 0x40000
#define PLANE_B 0x41000
#define PLANE_C 0x42000
#define PLANE_D 0x43000
#define BACK 0x7FFFE

/* NBG3's characters: four of one colour, a plane's each, and one of a colour for each of its
 * four cells */
#define CHARACTER_WORDS 128
#define FILL_A 1
#define FILL_B 2
#define FILL_C 3
#define FILL_D 4
#define QUARTERS 5

/* A page of NBG3 is 32 by 32 names of two words: the flips and a palette number, then a
 * character number, which counts 32 bytes of VRAM */
#define PAGE_NAMES 32
#define FLIP_ACROSS 0x4000
#define FLIP_DOWN 0x8000
#define CHARACTER_NUMBER(character) ((uint16_t)((CHARACTERS >> 5) + ((character) << 3)))

/* With 256 colours a palette number's top 3 bits count 256 colours. NBG3's colours start 256
 * colours in, so its palette 0 is colours 256 on and this one colours 512 on */
#define PALETTE_HIGH 0x10

/* NBG1's page is 64 names across. Its names are a palette and a character number of 12 bits */
#define CELL_PAGE_NAMES 64
#define CELL_PALETTE 2
#define CELL_NAME ((uint16_t)((CELL_PALETTE << 12) | (CELL >> 5)))

/* NBG3's scroll: the screen's middle, 160 across and 112 down, on the middle of a map of two
 * planes of 512 dots each way */
#define SCROLL_ACROSS (512 - 160)
#define SCROLL_DOWN (512 - 112)

static const vdp2_scroll_t format = {
        .nbg = {
                [1] = {
                        .colors = VDP2_COLORS_16,
                        .character = VDP2_CHARACTER_8,
                        .names = VDP2_NAMES_1_WORD_WIDE,
                        .plane = VDP2_PLANE_1X1,
                        .planes = { CELL_NAMES, CELL_NAMES, CELL_NAMES, CELL_NAMES }
                },
                [3] = {
                        .colors = VDP2_COLORS_256,
                        .character = VDP2_CHARACTER_16,
                        .names = VDP2_NAMES_2_WORDS,
                        .plane = VDP2_PLANE_1X1,
                        .planes = { PLANE_A, PLANE_B, PLANE_C, PLANE_D },
                        .color_base = 1
                }
        }
};

/* A character of NBG3 whose four cells are a colour each: top left, top right, bottom left,
 * bottom right. A dot is a byte */
static void
character(uint32_t number, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth)
{
        const uint32_t dots[4] = { first, second, third, fourth };
        uint16_t * const words = vdp2_vram(CHARACTERS) + (number * CHARACTER_WORDS);

        for (uint32_t word = 0; word < CHARACTER_WORDS; word++) {
                const uint32_t dot = dots[word / (CHARACTER_WORDS / 4)];

                words[word] = (uint16_t)((dot << 8) | dot);
        }
}

/* A page of NBG3 all one character, but for the name at a row and column */
static void
plane(uint32_t offset, uint32_t fill, uint32_t row, uint32_t column, uint32_t flips)
{
        uint16_t * const names = vdp2_vram(offset);
        const uint32_t at = ((row * PAGE_NAMES) + column) << 1;

        for (uint32_t name = 0; name < PAGE_NAMES * PAGE_NAMES; name++) {
                names[name << 1] = 0;
                names[(name << 1) + 1] = CHARACTER_NUMBER(fill);
        }
        names[at] = (uint16_t)(flips | PALETTE_HIGH);
        names[at + 1] = CHARACTER_NUMBER(QUARTERS);
}

int
main(void)
{
        uint16_t * const cell = vdp2_vram(CELL);
        uint16_t * const cell_names = vdp2_vram(CELL_NAMES);

        vdp2_start();

        *vdp2_cram(256 + FILL_A) = VDP2_RGB(31, 0, 0);
        *vdp2_cram(256 + FILL_B) = VDP2_RGB(0, 31, 0);
        *vdp2_cram(256 + FILL_C) = VDP2_RGB(0, 0, 31);
        *vdp2_cram(256 + FILL_D) = VDP2_RGB(31, 31, 31);
        *vdp2_cram(512 + 5) = VDP2_RGB(31, 31, 0);
        *vdp2_cram(512 + 6) = VDP2_RGB(0, 31, 31);
        *vdp2_cram(512 + 7) = VDP2_RGB(31, 0, 31);
        *vdp2_cram(512 + 8) = VDP2_RGB(16, 16, 16);
        *vdp2_cram((CELL_PALETTE << 4) + 1) = VDP2_RGB(31, 16, 0);
        hx_test_equal("cram.word", *(volatile uint16_t *)vdp2_cram(512 + 8), VDP2_RGB(16, 16, 16));

        /* Character 0 is empty, and its first 32 bytes are NBG1's empty cell too */
        character(0, 0, 0, 0, 0);
        character(FILL_A, FILL_A, FILL_A, FILL_A, FILL_A);
        character(FILL_B, FILL_B, FILL_B, FILL_B, FILL_B);
        character(FILL_C, FILL_C, FILL_C, FILL_C, FILL_C);
        character(FILL_D, FILL_D, FILL_D, FILL_D, FILL_D);
        character(QUARTERS, 5, 6, 7, 8);

        /* The four characters around the map's middle: as it is, flipped across, flipped
         * down and flipped both ways */
        plane(PLANE_A, FILL_A, PAGE_NAMES - 1, PAGE_NAMES - 1, 0);
        plane(PLANE_B, FILL_B, PAGE_NAMES - 1, 0, FLIP_ACROSS);
        plane(PLANE_C, FILL_C, 0, PAGE_NAMES - 1, FLIP_DOWN);
        plane(PLANE_D, FILL_D, 0, 0, FLIP_ACROSS | FLIP_DOWN);
        hx_test_equal("names.word", *(volatile uint16_t *)vdp2_vram(PLANE_D),
            FLIP_ACROSS | FLIP_DOWN | PALETTE_HIGH);

        /* NBG1: a cell of colour 1 at the screen's top left and bottom right */
        for (uint32_t word = 0; word < 16; word++) {
                cell[word] = 0x1111;
        }
        for (uint32_t name = 0; name < CELL_PAGE_NAMES * CELL_PAGE_NAMES; name++) {
                cell_names[name] = 0;
        }
        cell_names[0] = CELL_NAME;
        cell_names[(27 * CELL_PAGE_NAMES) + 39] = CELL_NAME;

        vdp2_scroll_format(&format);
        /* No call sets a scroll: NBG3's is written through the map */
        VDP2->scxn3 = SCROLL_ACROSS;
        VDP2->scyn3 = SCROLL_DOWN;

        /* Half A: NBG1's names and its patterns, and the two reads a cycle of NBG3's
         * patterns. Half B: NBG3's names */
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG1_NAMES, VDP2_ACCESS_NBG1_PATTERNS,
            VDP2_ACCESS_NBG3_PATTERNS, VDP2_ACCESS_NONE, VDP2_ACCESS_NBG3_PATTERNS,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_cycle(VDP2_BANK_B0, VDP2_CYCLE(VDP2_ACCESS_NBG3_NAMES, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_nbg_priority(0, 3, 0, 2);
        vdp2_show(VDP2_NBG1 | VDP2_NBG3);

        vdp2_back_color(BACK, VDP2_RGB(0, 0, 0));
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);

        hx_test_end();
        for (;;) {
        }
}
