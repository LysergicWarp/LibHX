#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* hx_test_show's picture.
 *
 * With ROWS, of a report the test writes by hand. ROWS is that report's rows in order, a
 * letter each: p for a row that passes and F for one that fails. FULL adds the line of a full
 * report. The text is put into the report, shown, and taken out again. The test's own rows
 * follow, so the report the run reads is theirs, and the picture it compares is the text's.
 *
 * With OWN, of the test's own report: OWN rows that pass, shown once the report is closed. */

#if defined(ROWS) == defined(OWN)
#error One of ROWS and OWN is defined
#endif

#ifdef ROWS

#define TEXT_OF(letters) #letters
#define TEXT(letters) TEXT_OF(letters)

/* The text starts past the mark */
#define MARK_SIZE (sizeof HX_TEST_MARK - 1)

static const char rows[] = TEXT(ROWS);

static uint32_t
put(uint32_t at, const char *text)
{
        while (*text != '\0') {
                hx_test_report[at] = *text;
                at++;
                text++;
        }
        return at;
}

int
main(void)
{
        uint32_t at = MARK_SIZE;

        for (const char *row = rows; *row != '\0'; row++) {
                at = put(at, *row == 'p' ? "pass row 00000000 00000000\n"
                    : "FAIL row 00000001 00000000\n");
        }
#ifdef FULL
        at = put(at, "full\n");
#endif
        hx_test_show();
        while (at > MARK_SIZE) {
                at--;
                hx_test_report[at] = 0;
        }

        /* The display on at 320 by 224, with a black border */
        hx_test_equal("tvmd", VDP2->tvmd, VDP2_TVMD_DISP);

        /* The table: a black line at the top, and the first row's band from the third line */
        hx_test_equal("table.top", vdp2_vram(0)[0], VDP2_RGB(0, 0, 0));
        hx_test_equal("table.first", vdp2_vram(0)[2],
            rows[0] == 'p' ? VDP2_RGB(0, 31, 0) : VDP2_RGB(31, 0, 0));

        hx_test_end();
        for (;;) {
        }
}

#else

int
main(void)
{
        for (uint32_t row = 0; row < OWN; row++) {
                hx_test_equal("row", row, row);
        }
        hx_test_end();
        hx_test_show();
        for (;;) {
        }
}

#endif
