#ifndef HX_TEXT_H
#define HX_TEXT_H

#include <stdint.h>

#include <hx/font.h>
#include <hx/vdp2.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * Text on a scroll screen of VDP2: a glyph is a cell, and a character on screen is a pattern
 * name.
 *
 *     const hx_text_t text = {
 *             .page = vdp2_vram(NAMES),
 *             .names = VDP2_NAMES_1_WORD_FLIP,
 *             .first = CELLS >> 5
 *     };
 *
 *     hx_text_cells(hx_font, vdp2_vram(CELLS), 1, 0);
 *     hx_text_string(&text, 2, 1, "Hello");
 *
 * hx_text_cells makes a font's glyphs into cells, once. The other calls write pattern names
 * that show those cells: a string, a number, a run of one character.
 *
 * The calls write memory and no register of VDP2. The screen is the program's to set up, with
 * <hx/vdp2.h>: 16 colours, characters of 8 by 8 dots, and the page, the cells and the size of
 * a pattern name as the text has them. A write to VRAM while the screen shows waits for a
 * timing the bank's cycle leaves the CPU.
 *
 * The calls keep no state, and there is no cursor: a call is told its column and row, and
 * returns how many characters it wrote. Calls from an interrupt handler or from the second
 * SH-2 are the program's to keep apart from others that write the same memory. */

/** Makes cells of a font's glyphs: 16 colours, 4 bits a dot, 32 bytes a cell, the glyphs one
 * after another from cells on. A dot of a glyph is ink and every other dot paper, each the
 * number of a colour in the cell's palette, 0 to 15. More bits of either are dropped. A dot
 * of 0 is transparent on a screen that has transparent dots.
 *
 * It returns how many cells it wrote: the font's count of glyphs.
 *
 * A font whose glyphs are not 8 by 8 dots gives no cell: nothing is written, and 0 comes
 * back. A null font is the primary one, hx_font.
 *
 * cells is not checked for a null pointer. */
uint32_t hx_text_cells(const hx_font_t *font, uint16_t *cells, uint32_t ink, uint32_t paper);

/** Where text goes and what it is made of: a page of pattern names, and the cells of a font.
 *
 * A page is 64 by 64 pattern names, for a screen whose characters are 8 by 8 dots. Text in
 * two colours, or in two fonts, is two of these on the same page. */
typedef struct {
        /** The font the cells were made of, for the place of a code's glyph among them. A
         * null pointer is the primary font, hx_font. */
        const hx_font_t *font;
        /** The page's first pattern name. It is not checked for a null pointer. */
        uint16_t *page;
        /** The size of a pattern name, as the screen has it. A value the enumeration does not
         * name counts by its bit 1, as the chip takes it: two words where it is 0. */
        vdp2_names_t names;
        /** The pattern name of the font's first glyph: its cell's character number, the
         * palette, and any other bit the name is to have. The name of another glyph is this
         * plus the glyph's count from the first, so the cells follow one another as
         * hx_text_cells writes them.
         *
         * A name of one word is the low 16 bits. A name of two words is all 32, the first
         * word in the top half. A sum past the character number's bits carries into the bits
         * above them.
         *
         * A name of one word holds the low 10 or 12 bits of the character number alone. The
         * bits above them are the screen's supplement (vdp2_nbg_t), not a part of this. */
        uint32_t first;
} hx_text_t;

/** @name Writing text
 * Each call writes a pattern name for a character, from a column and row of the page on, and
 * returns how many characters it wrote.
 *
 * The first character's place is row * 64 + column, counted from the page's first name. A
 * place wraps at the page's 4096 names, and the characters follow one another by place: after
 * column 63 comes column 0 of the row below, and after the page's last name its first.
 *
 * A code the font does not hold shows the font's fallback glyph.
 *
 * text is not checked for a null pointer. */
/** @{ */

/** Writes a string: its characters up to the 0 that ends it, each a code of 0 to 255.
 *
 * string is not checked for a null pointer. */
uint32_t hx_text_string(const hx_text_t *text, uint32_t column, uint32_t row,
    const char *string);

/** Writes one character count times: a line, or a run of spaces over what was there. The
 * code's low 8 bits are the character. A count of more than a page's 4096 is 4096. */
uint32_t hx_text_fill(const hx_text_t *text, uint32_t column, uint32_t row, uint32_t code,
    uint32_t count);

/** Writes the low digits hexadecimal digits of a value, the highest first, with A to F in
 * upper case. More than 8 digits are 8, and 0 digits write nothing. */
uint32_t hx_text_hex(const hx_text_t *text, uint32_t column, uint32_t row, uint32_t value,
    uint32_t digits);

/** Writes a value in decimal, with no sign and no leading 0: 1 to 10 digits. */
uint32_t hx_text_decimal(const hx_text_t *text, uint32_t column, uint32_t row,
    uint32_t value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !HX_TEXT_H */
