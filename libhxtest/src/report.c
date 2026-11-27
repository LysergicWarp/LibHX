#include <hx/test.h>

#include <stdbool.h>
#include <stddef.h>

#define MARK_SIZE 8

/* Room kept for "full", the last line and the 0 */
#define END_SIZE 32

/* Number, space */
#define FIELD_SIZE 9

/* Written a byte at a time and in order, the mark last: whoever reads it is outside the
 * program */
volatile char hx_test_report[HX_TEST_REPORT_SIZE];

/* The text's bytes so far */
static size_t used;
static uint32_t rows;
static uint32_t failed;
static bool full;
static bool ended;

static void
put(char character)
{
        hx_test_report[MARK_SIZE + used] = character;
        used++;
}

static void
put_text(const char *text)
{
        while (*text != '\0') {
                put(*text);
                text++;
        }
}

static void
put_number(uint32_t number)
{
        for (int shift = 28; shift >= 0; shift -= 4) {
                put("0123456789abcdef"[(number >> shift) & 15U]);
        }
}

static size_t
length(const char *text)
{
        size_t count = 0;

        while (text[count] != '\0') {
                count++;
        }
        return count;
}

void
hx_test_equal(const char *name, uint32_t got, uint32_t wanted)
{
        const bool pass = got == wanted;
        /* "pass ", the name and its space, two numbers with a space or the line's end */
        const size_t size = 5 + length(name) + 1 + (2 * FIELD_SIZE);

        if (ended) {
                return;
        }
        rows++;
        if (!pass) {
                failed++;
        }
        if (size > HX_TEST_REPORT_SIZE - MARK_SIZE - END_SIZE - used) {
                full = true;
                return;
        }
        put_text(pass ? "pass " : "FAIL ");
        put_text(name);
        put(' ');
        put_number(got);
        put(' ');
        put_number(wanted);
        put('\n');
}

void
hx_test_note(const char *name, uint32_t value)
{
        /* "note ", the name and its space, the number with the line's end */
        const size_t size = 5 + length(name) + 1 + FIELD_SIZE;

        if (ended) {
                return;
        }
        if (size > HX_TEST_REPORT_SIZE - MARK_SIZE - END_SIZE - used) {
                full = true;
                return;
        }
        put_text("note ");
        put_text(name);
        put(' ');
        put_number(value);
        put('\n');
}

void
hx_test_end(void)
{
        if (ended) {
                return;
        }
        ended = true;
        if (full) {
                put_text("full\n");
        }
        put_text("end ");
        put_number(rows);
        put(' ');
        put_number(failed);
        put('\n');
        put('\0');
        for (size_t at = 0; at < MARK_SIZE; at++) {
                hx_test_report[at] = HX_TEST_MARK[at];
        }
}
