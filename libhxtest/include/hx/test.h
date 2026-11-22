#ifndef HX_TEST_H
#define HX_TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * A test disc's report: text in work RAM, read from outside once its mark is there.
 *
 * The report is hx_test_report, HX_TEST_REPORT_SIZE bytes: 8 for the mark, then the text and
 * a 0. The mark is HX_TEST_MARK, written last, so a report that is there is whole.
 *
 * The text is a row a line, then the last line:
 *
 *     pass <name> <got> <wanted>
 *     FAIL <name> <got> <wanted>
 *     end <rows> <failed>
 *
 * The numbers are 8 hexadecimal digits. A line "full" ahead of the last says rows were left
 * out for want of room. They are counted all the same.
 *
 * hx_test_show puts the rows on screen, for a run on a console. */
#define HX_TEST_REPORT_SIZE 0x4000
#define HX_TEST_MARK "HXREPORT"

/** The report. Every byte past the text so far is 0 */
extern volatile char hx_test_report[HX_TEST_REPORT_SIZE];

/** A row, which passes when got is wanted. The name holds no space and no line end */
void hx_test_equal(const char *name, uint32_t got, uint32_t wanted);

/** The last line, then the mark. The report is closed: a call of hx_test_equal or of this
 * after it does nothing */
void hx_test_end(void);

/** The rows a picture of hx_test_show holds */
#define HX_TEST_SHOW_ROWS 40

/** The report's rows so far on screen, a band of colour for each from the top down: green for
 * a row that passes and red for one that fails. The screen is 320 by 224, black elsewhere.
 *
 * - A band is 4 lines, the first from the third line of the screen. A black line parts two
 *   bands, and 3 black lines follow every fifth.
 * - The first HX_TEST_SHOW_ROWS rows have a band. With more rows, one more band below the
 *   others says so: white when every row without a band passes, red when one fails.
 * - That band is red too for a full report, once hx_test_end has written its line "full":
 *   rows are missing from it.
 *
 * The call takes VDP2 as its own: it starts it as vdp2_start does and writes the first 448
 * bytes of VRAM. A picture the program had is gone.
 *
 * The rows are read from the report's text, at any time: ahead of hx_test_end as after it. */
void hx_test_show(void);

#ifdef __cplusplus
}
#endif

#endif /* !HX_TEST_H */
