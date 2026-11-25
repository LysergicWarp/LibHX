#include <hx/text.h>

/* A page is 64 by 64 pattern names */
#define PAGE_SHIFT 6
#define PAGE_NAMES 4096

uint32_t
hx_text_cells(const hx_font_t *font, uint16_t *cells, uint32_t ink, uint32_t paper)
{
        /* The four dots of a word, for each value of 4 bits of a glyph's row */
        uint16_t dots[16];
        const uint8_t *row;

        if (!font) {
                font = hx_font;
        }
        if (font->width != 8 || font->height != 8) {
                return 0;
        }

        ink &= 15;
        paper &= 15;
        for (uint32_t bits = 0; bits < 16; bits++) {
                uint32_t word = 0;

                for (uint32_t bit = 8; bit; bit >>= 1) {
                        word = (word << 4) | ((bits & bit) ? ink : paper);
                }
                dots[bits] = (uint16_t)word;
        }

        /* A row of 8 dots is two words, its left half first */
        row = font->glyphs;
        for (uint32_t rows = (uint32_t)font->count << 3; rows; rows--) {
                const uint32_t bits = *row++;

                *cells++ = dots[bits >> 4];
                *cells++ = dots[bits & 15];
        }
        return font->count;
}

/* A text's font, and whether its pattern names are two words */
typedef struct {
        const hx_text_t *text;
        const hx_font_t *font;
        uint32_t two_words;
} writer_t;

static writer_t
writer(const hx_text_t *text)
{
        const writer_t to = {
                .text = text,
                .font = text->font ? text->font : hx_font,
                .two_words = ((uint32_t)text->names & 2) == 0
        };

        return to;
}

/* The pattern name of a code's glyph, at a place of the page */
static void
put(const writer_t *to, uint32_t place, uint32_t code)
{
        uint32_t glyph = (code & 0xFF) - to->font->first;
        uint32_t name;

        /* A code below the first glyph's wraps to a count past every glyph */
        if (glyph >= to->font->count) {
                glyph = to->font->fallback;
        }
        name = to->text->first + glyph;

        place &= PAGE_NAMES - 1;
        if (to->two_words) {
                to->text->page[place << 1] = (uint16_t)(name >> 16);
                to->text->page[(place << 1) + 1] = (uint16_t)name;
        } else {
                to->text->page[place] = (uint16_t)name;
        }
}

uint32_t
hx_text_string(const hx_text_t *text, uint32_t column, uint32_t row, const char *string)
{
        const writer_t to = writer(text);
        const uint32_t place = (row << PAGE_SHIFT) + column;
        uint32_t count = 0;

        while (string[count]) {
                put(&to, place + count, (uint8_t)string[count]);
                count++;
        }
        return count;
}

uint32_t
hx_text_fill(const hx_text_t *text, uint32_t column, uint32_t row, uint32_t code,
    uint32_t count)
{
        const writer_t to = writer(text);
        const uint32_t place = (row << PAGE_SHIFT) + column;

        if (count > PAGE_NAMES) {
                count = PAGE_NAMES;
        }
        for (uint32_t at = 0; at < count; at++) {
                put(&to, place + at, code);
        }
        return count;
}

uint32_t
hx_text_hex(const hx_text_t *text, uint32_t column, uint32_t row, uint32_t value,
    uint32_t digits)
{
        static const char codes[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };
        const writer_t to = writer(text);
        const uint32_t place = (row << PAGE_SHIFT) + column;

        if (digits > 8) {
                digits = 8;
        }
        for (uint32_t at = 0; at < digits; at++) {
                const uint32_t shift = (digits - 1 - at) << 2;

                put(&to, place + at, (uint8_t)codes[(value >> shift) & 15]);
        }
        return digits;
}

uint32_t
hx_text_decimal(const hx_text_t *text, uint32_t column, uint32_t row, uint32_t value)
{
        const writer_t to = writer(text);
        const uint32_t place = (row << PAGE_SHIFT) + column;
        /* The digits from the lowest up: 10 hold every value of 32 bits */
        uint8_t codes[10];
        uint32_t count = 0;

        do {
                codes[count++] = (uint8_t)('0' + (value % 10));
                value /= 10;
        } while (value);

        for (uint32_t at = 0; at < count; at++) {
                put(&to, place + at, codes[count - 1 - at]);
        }
        return count;
}
