#include <hx/test.h>

#include <stdbool.h>
#include <stddef.h>

#include <hx/vdp2.h>

/* The text starts past the mark */
#define MARK_SIZE (sizeof HX_TEST_MARK - 1)

/* The picture: the back screen with a colour for each line, its table at the start of VRAM */
#define TABLE 0
#define SCREEN_LINES 224

/* In lines. A group is five rows, each band but the last with a line below it */
#define TOP 2
#define BAND 4
#define BETWEEN 1
#define GROUP_ROWS 5
#define GROUP_GAP 3

/* The band that says there are more rows, a gap below the last row's */
#define MORE_ROW HX_TEST_BANDS_ROWS

#define BLACK VDP2_RGB(0, 0, 0)
#define PASS VDP2_RGB(0, 31, 0)
#define FAIL VDP2_RGB(31, 0, 0)
#define MORE VDP2_RGB(31, 31, 31)

/* The first line of a row's band */
#define BAND_TOP(row) \
        (TOP + ((row) * (BAND + BETWEEN)) + (((row) / GROUP_ROWS) * (GROUP_GAP - BETWEEN)))

static_assert(MORE_ROW % GROUP_ROWS == 0, "The band for more rows does not follow a whole group");
static_assert(BAND_TOP(MORE_ROW) + BAND <= SCREEN_LINES, "The bands are more lines than the screen");

static void
band(uint16_t *table, uint32_t row, uint16_t color)
{
        const uint32_t top = BAND_TOP(row);

        for (uint32_t line = 0; line < BAND; line++) {
                table[top + line] = color;
        }
}

void
hx_test_bands(void)
{
        uint16_t * const table = vdp2_vram(TABLE);
        uint32_t rows = 0;
        bool more_failed = false;
        size_t at = MARK_SIZE;

        vdp2_start();
        for (uint32_t line = 0; line < SCREEN_LINES; line++) {
                table[line] = BLACK;
        }

        /* A row's line starts with pass or FAIL, and a note's with note. The lines end at one
         * that is none of them: "full", the last line, or the 0 past the text so far */
        while (at < HX_TEST_REPORT_SIZE) {
                const char first = hx_test_report[at];

                if (first != 'p' && first != 'F' && first != 'n') {
                        break;
                }
                if (first != 'n') {
                        if (rows < HX_TEST_BANDS_ROWS) {
                                band(table, rows, first == 'p' ? PASS : FAIL);
                        } else if (first == 'F') {
                                more_failed = true;
                        }
                        rows++;
                }
                while (at < HX_TEST_REPORT_SIZE && hx_test_report[at] != '\n') {
                        at++;
                }
                at++;
        }

        /* A full report is short of lines, whatever their count */
        if (at < HX_TEST_REPORT_SIZE && hx_test_report[at] == 'f') {
                band(table, MORE_ROW, FAIL);
        } else if (rows > HX_TEST_BANDS_ROWS) {
                band(table, MORE_ROW, more_failed ? FAIL : MORE);
        }

        vdp2_back_lines(TABLE);
        vdp2_screen(VDP2_WIDTH_320, VDP2_HEIGHT_224, VDP2_INTERLACE_NONE, VDP2_BORDER_BLACK);
}
