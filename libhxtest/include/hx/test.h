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
 * The text is a line for each row and each note, then the last line:
 *
 *     pass <name> <got> <wanted>
 *     FAIL <name> <got> <wanted>
 *     note <name> <value>
 *     end <rows> <failed>
 *
 * The numbers are 8 hexadecimal digits. A note is a value to read, with nothing to pass or
 * fail: it is no row, and the last line does not count it. A line "full" ahead of the last
 * says rows or notes were left out for want of room. The rows are counted all the same.
 *
 * hx_test_show puts the report on screen as text, for a run on a console. hx_test_bands does
 * it without a font or a scroll screen.
 *
 * hx_test_pad_temp reads a Saturn pad's buttons, for the time being. */
#define HX_TEST_REPORT_SIZE 0x4000
#define HX_TEST_MARK "HXREPORT"

/** The report. Every byte past the text so far is 0 */
extern volatile char hx_test_report[HX_TEST_REPORT_SIZE];

/** A row, which passes when got is wanted. The name holds no space and no line end */
void hx_test_equal(const char *name, uint32_t got, uint32_t wanted);

/** A note: a value to read in the report, with nothing to pass or fail. It is not a row. The
 * name holds no space and no line end */
void hx_test_note(const char *name, uint32_t value);

/** The last line, then the mark. The report is closed: a call of hx_test_equal, of
 * hx_test_note or of this after it does nothing */
void hx_test_end(void);

/** The report so far on screen as text, in the primary font: 40 columns by 28 lines on a
 * screen of 320 by 224, black behind the text and around the picture.
 *
 * - The first line counts the rows the text holds and the ones of them that fail. It is
 *   green when every row passes, and red when one fails. It is red too for a full report,
 *   and says so, once hx_test_end has written its line "full": lines are missing from it,
 *   and the counts leave their rows out.
 * - A line follows for each row and each note: the rows that fail first, in red, then the
 *   notes, in white, then the rows that pass, in green. Each kind keeps the report's order.
 * - A line is the first 21 characters of the name, then the number: what the row got, or the
 *   note's value. A row that fails has what was wanted after it.
 * - 27 such lines fit. With more, 26 are shown and the last line of the screen counts the
 *   others: in red when a row that fails is among them, in white otherwise.
 *
 * The call takes VDP2 as its own: it starts it as vdp2_start does, shows NBG0 alone, and
 * writes within the first 32 kbytes of VRAM, and colours 1, 17 and 33 of the colour RAM. A
 * picture the program had is gone. The primary font is 8 by 8 dots: with another size no text
 * shows, and the screen is black.
 *
 * The lines are read from the report's text, at any time: ahead of hx_test_end as after it. */
void hx_test_show(void);

/** The rows a picture of hx_test_bands holds */
#define HX_TEST_BANDS_ROWS 40

/** The report's rows so far on screen, a band of colour for each from the top down: green for
 * a row that passes and red for one that fails. The screen is 320 by 224, black elsewhere.
 * The picture is the back screen's alone, with no font and no scroll screen: it shows where
 * hx_test_show's text does not. Notes have no band.
 *
 * - A band is 4 lines, the first from the third line of the screen. A black line parts two
 *   bands, and 3 black lines follow every fifth.
 * - The first HX_TEST_BANDS_ROWS rows have a band. With more rows, one more band below the
 *   others says so: white when every row without a band passes, red when one fails.
 * - That band is red too for a full report, once hx_test_end has written its line "full":
 *   lines are missing from it.
 *
 * The call takes VDP2 as its own: it starts it as vdp2_start does and writes the first 448
 * bytes of VRAM. A picture the program had is gone.
 *
 * The rows are read from the report's text, at any time: ahead of hx_test_end as after it. */
void hx_test_bands(void);

/** The buttons in hx_test_pad_temp's word, a bit set for one that is held */
#define HX_TEST_PAD_RIGHT_TEMP 0x8000
#define HX_TEST_PAD_LEFT_TEMP 0x4000
#define HX_TEST_PAD_DOWN_TEMP 0x2000
#define HX_TEST_PAD_UP_TEMP 0x1000
#define HX_TEST_PAD_START_TEMP 0x0800
#define HX_TEST_PAD_A_TEMP 0x0400
#define HX_TEST_PAD_C_TEMP 0x0200
#define HX_TEST_PAD_B_TEMP 0x0100
#define HX_TEST_PAD_R_TEMP 0x0080
#define HX_TEST_PAD_X_TEMP 0x0040
#define HX_TEST_PAD_Y_TEMP 0x0020
#define HX_TEST_PAD_Z_TEMP 0x0010
#define HX_TEST_PAD_L_TEMP 0x0008

/** Every button of the word */
#define HX_TEST_PAD_BUTTONS_TEMP 0xFFF8

/** Set in the word when a Saturn pad answered */
#define HX_TEST_PAD_THERE_TEMP 0x0001

/** The buttons held on the Saturn pad at a port, 0 for the first port and any other number
 * for the second: HX_TEST_PAD_THERE_TEMP and a bit for each button held. 0 when what is at
 * the port does not answer as a Saturn pad, or nothing is there.
 *
 * Temporary. It is here for tests until the SDK reads peripherals through the SMPC, and is
 * removed then.
 *
 * The call reads the pad's lines itself, through the SMPC's parallel I/O registers. It gives
 * both ports to the SH-2 and leaves them so, with EXLE off: the SMPC reads no peripheral
 * after it. A Mega Drive pad, a mouse, a multitap and every other peripheral give 0.
 *
 * A read takes the port's lines through four states and back to rest. Not for an interrupt
 * handler that may come in the middle of another read. */
uint32_t hx_test_pad_temp(uint32_t port);

#ifdef __cplusplus
}
#endif

#endif /* !HX_TEST_H */
