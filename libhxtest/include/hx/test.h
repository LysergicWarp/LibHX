#ifndef HX_TEST_H
#define HX_TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A test disc's report: text in work RAM, read from outside once its mark is there.
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
 * out for want of room. They are counted all the same. */
#define HX_TEST_REPORT_SIZE 0x4000
#define HX_TEST_MARK "HXREPORT"

/* A row, which passes when got is wanted. The name holds no space and no line end */
void hx_test_equal(const char *name, uint32_t got, uint32_t wanted);

/* The last line, then the mark. The report is closed: a call of either function after this
 * does nothing */
void hx_test_end(void);

#ifdef __cplusplus
}
#endif

#endif /* !HX_TEST_H */
