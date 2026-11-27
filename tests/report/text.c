#include <stdint.h>

#include <hx/test.h>
#include <hx/vdp2.h>

/* hx_test_show's picture.
 *
 * With ROWS, of a report the test writes by hand. ROWS is that report's lines in order, a
 * letter each, and a line's name ends in its place among them, two digits from 00:
 *
 *     p   pass row.<place> <place> <place>
 *     F   FAIL bad.<place> <place> <place + 0x100>
 *     n   note value.<place> <0xabcd0000 + place>
 *     L   pass a.name.past.21.columns.<place> 12345678 12345678
 *
 * FULL adds the line of a full report. The text is put into the report, shown, and taken out
 * again. The test's own rows follow, so the report the run reads is theirs, and the picture
 * it compares is the text's.
 *
 * With OWN, of the test's own report: a note and OWN rows that pass, shown once the report is
 * closed. */

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

static uint32_t
put_place(uint32_t at, uint32_t place)
{
        hx_test_report[at] = (char)('0' + (place / 10));
        hx_test_report[at + 1] = (char)('0' + (place % 10));
        return at + 2;
}

static uint32_t
put_number(uint32_t at, uint32_t number)
{
        for (int shift = 28; shift >= 0; shift -= 4) {
                hx_test_report[at] = "0123456789abcdef"[(number >> shift) & 15U];
                at++;
        }
        return at;
}

int
main(void)
{
        uint32_t at = MARK_SIZE;
        uint32_t place = 0;

        for (const char *row = rows; *row != '\0'; row++, place++) {
                switch (*row) {
                case 'p':
                        at = put_place(put(at, "pass row."), place);
                        at = put_number(put(at, " "), place);
                        at = put_number(put(at, " "), place);
                        break;
                case 'F':
                        at = put_place(put(at, "FAIL bad."), place);
                        at = put_number(put(at, " "), place);
                        at = put_number(put(at, " "), place + 0x100);
                        break;
                case 'n':
                        at = put_place(put(at, "note value."), place);
                        at = put_number(put(at, " "), 0xABCD0000 + place);
                        break;
                default:
                        at = put_place(put(at, "pass a.name.past.21.columns."), place);
                        at = put(at, " 12345678 12345678");
                        break;
                }
                at = put(at, "\n");
        }
#ifdef FULL
        at = put(at, "full\n");
#endif
        hx_test_show();
        while (at > MARK_SIZE) {
                at--;
                hx_test_report[at] = 0;
        }

        /* The display on at 320 by 224, with a black border. VRAM is the screen's from here
         * on, with no timing for the CPU, so no row reads it */
        hx_test_equal("tvmd", VDP2->tvmd, VDP2_TVMD_DISP);

        /* The three inks */
        hx_test_equal("ink.white", *vdp2_cram(1), VDP2_RGB(31, 31, 31));
        hx_test_equal("ink.green", *vdp2_cram(17), VDP2_RGB(0, 31, 0));
        hx_test_equal("ink.red", *vdp2_cram(33), VDP2_RGB(31, 0, 0));

        hx_test_end();
        for (;;) {
        }
}

#else

int
main(void)
{
        hx_test_note("count", OWN);
        for (uint32_t row = 0; row < OWN; row++) {
                hx_test_equal("row", row, row);
        }
        hx_test_end();
        hx_test_show();
        for (;;) {
        }
}

#endif
