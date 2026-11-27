#include <hx/test.h>

#include <stdbool.h>
#include <stddef.h>

#include <hx/text.h>
#include <hx/vdp2.h>

/* The text starts past the mark */
#define MARK_SIZE (sizeof HX_TEST_MARK - 1)

/* VRAM, half A: the page of pattern names, the back screen's colour, and the primary font's
 * glyphs as cells */
#define NAMES 0x00000
#define BACK 0x02000
#define CELLS 0x04000

/* A page is 64 by 64 pattern names */
#define PAGE_NAMES 4096

/* The screen in characters */
#define COLUMNS 40
#define LINES 28

/* A line: the name, then the number a row got or a note's value, then what a row wanted */
#define NAME_COLUMNS 21
#define GOT_COLUMN 22
#define WANTED_COLUMN 31
#define DIGITS 8

/* The first line counts, and the last does when the lines are more than fit */
#define FIRST_LINE 1
#define FIT (LINES - 1)
#define FIT_WITH_MORE (LINES - 2)

/* "pass ", "FAIL " and "note " */
#define KIND_SIZE 5

/* A palette for each colour of text, its colour 1 the ink */
#define WHITE 0
#define GREEN 1
#define RED 2
#define INK(palette) (((palette) << 4) + 1)

/* A pattern name of one word: a palette in the top 4 bits, then a character number, which
 * counts 32 bytes of VRAM */
#define NAME(palette) ((uint32_t)(((palette) << 12) | (CELLS >> 5)))

static_assert(WANTED_COLUMN + DIGITS <= COLUMNS, "A line is more columns than the screen");

/* A line of the report by its first letter: a row that passes, one that fails, a note, or
 * none of them, which is "full", the last line, or the 0 past the text so far */
typedef enum {
        PASS,
        FAIL,
        NOTE,
        KINDS,
        OTHER = KINDS
} kind_t;

/* The order on screen, and each kind's colour */
static const kind_t order[KINDS] = { FAIL, NOTE, PASS };
static const uint8_t palettes[KINDS] = { [PASS] = GREEN, [FAIL] = RED, [NOTE] = WHITE };

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

static kind_t
kind(size_t at)
{
        if (at >= HX_TEST_REPORT_SIZE) {
                return OTHER;
        }
        switch (hx_test_report[at]) {
        case 'p':
                return PASS;
        case 'F':
                return FAIL;
        case 'n':
                return NOTE;
        default:
                return OTHER;
        }
}

/* The place of the line after the one at a place */
static size_t
next(size_t at)
{
        while (at < HX_TEST_REPORT_SIZE && hx_test_report[at] != '\n') {
                at++;
        }
        return at + 1;
}

/* Copies a field of a line, its characters up to a space or the line's end, count of them at
 * most. It returns the place past the field, and past its space where it has one */
static size_t
field(char *to, uint32_t count, size_t at)
{
        while (at < HX_TEST_REPORT_SIZE) {
                const char character = hx_test_report[at];

                if (character == ' ') {
                        return at + 1;
                }
                if (character == '\n' || character == '\0') {
                        break;
                }
                if (count) {
                        *to++ = character;
                        count--;
                }
                at++;
        }
        return at;
}

/* The text of a palette's colour */
static hx_text_t
ink(uint32_t palette)
{
        const hx_text_t text = {
                .page = vdp2_vram(NAMES),
                .names = VDP2_NAMES_1_WORD_FLIP,
                .first = NAME(palette)
        };

        return text;
}

/* The report's line at a place, on a line of the screen */
static void
line(const hx_text_t *text, uint32_t row, size_t at, kind_t of)
{
        char characters[COLUMNS + 1];
        uint32_t end = GOT_COLUMN + DIGITS;

        for (uint32_t column = 0; column < COLUMNS; column++) {
                characters[column] = ' ';
        }
        at = field(characters, NAME_COLUMNS, at + KIND_SIZE);
        at = field(&characters[GOT_COLUMN], DIGITS, at);
        if (of == FAIL) {
                field(&characters[WANTED_COLUMN], DIGITS, at);
                end = WANTED_COLUMN + DIGITS;
        }
        characters[end] = '\0';
        hx_text_string(text, 0, row, characters);
}

void
hx_test_show(void)
{
        uint32_t counts[KINDS] = { 0 };
        uint32_t lines;
        uint32_t shown;
        uint32_t row = FIRST_LINE;
        uint32_t column;
        uint32_t cells;
        bool full;
        size_t at;

        for (at = MARK_SIZE; kind(at) != OTHER; at = next(at)) {
                counts[kind(at)]++;
        }
        /* A full report is short of lines, whatever their count */
        full = at < HX_TEST_REPORT_SIZE && hx_test_report[at] == 'f';
        lines = counts[PASS] + counts[FAIL] + counts[NOTE];
        shown = lines <= FIT ? lines : FIT_WITH_MORE;

        vdp2_start();

        *vdp2_cram(INK(WHITE)) = VDP2_RGB(31, 31, 31);
        *vdp2_cram(INK(GREEN)) = VDP2_RGB(0, 31, 0);
        *vdp2_cram(INK(RED)) = VDP2_RGB(31, 0, 0);
        vdp2_back_color(BACK, VDP2_RGB(0, 0, 0));

        {
                const hx_text_t text = ink(WHITE);

                cells = hx_text_cells(NULL, vdp2_vram(CELLS), 1, 0);
                hx_text_fill(&text, 0, 0, ' ', PAGE_NAMES);
        }

        {
                const hx_text_t text = ink(counts[FAIL] || full ? RED : GREEN);

                column = hx_text_string(&text, 0, 0, "rows ");
                column += hx_text_decimal(&text, column, 0, counts[PASS] + counts[FAIL]);
                column += hx_text_string(&text, column, 0, ", failed ");
                column += hx_text_decimal(&text, column, 0, counts[FAIL]);
                if (full) {
                        hx_text_string(&text, column, 0, ", report full");
                }
        }

        for (uint32_t each = 0; each < KINDS; each++) {
                const kind_t now = order[each];
                const hx_text_t text = ink(palettes[now]);

                for (at = MARK_SIZE; kind(at) != OTHER && row < FIRST_LINE + shown; at = next(at)) {
                        if (kind(at) == now) {
                                line(&text, row, at, now);
                                row++;
                        }
                }
        }

        /* The rows that fail come first, so one is left out where they are more than shown */
        if (lines > shown) {
                const hx_text_t text = ink(counts[FAIL] > shown ? RED : WHITE);

                column = hx_text_decimal(&text, 0, LINES - 1, lines - shown);
                hx_text_string(&text, column, LINES - 1, " more not shown");
        }

        vdp2_scroll_format(&format);
        /* NBG0's two reads, both from half A */
        vdp2_cycle(VDP2_BANK_A0, VDP2_CYCLE(VDP2_ACCESS_NBG0_NAMES, VDP2_ACCESS_NBG0_PATTERNS,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE, VDP2_ACCESS_NONE,
            VDP2_ACCESS_NONE, VDP2_ACCESS_NONE));
        vdp2_nbg_priority(1, 0, 0, 0);
        /* A font that gives no cell leaves that place of VRAM as it was, so the screen stays
         * off and the back screen alone shows */
        vdp2_show(cells ? VDP2_NBG0 : 0);

        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);
}
