#ifndef HX_FONT_H
#define HX_FONT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * A font: its glyphs as bits, for the text calls and for a program's own drawing.
 *
 * The SDK's fonts are in fonts/, a header each: <hx/font/unscii8.h>. A program links fonts of
 * its own beside them, as objects of its sources, and gives them to the calls that take a
 * font.
 *
 * One of the SDK's fonts is the primary one, hx_font: the font a call takes when it is given
 * none. The program's Makefile names it as HX_FONT (build/libs.mk). */

/** A font: the glyphs of the codes first to first + count - 1, one after another.
 *
 * A glyph is height rows from the top. A row is its dots from the left, 8 to a byte: the
 * byte's top bit is the left dot, 1 for a dot of the glyph and 0 for none. A row whose width
 * is no multiple of 8 ends on a byte's boundary all the same. */
typedef struct {
        /** The glyphs' rows. No call checks it for a null pointer. */
        const uint8_t *glyphs;
        /** How many glyphs the font holds. A code is a byte, so those past code 255 are not
         * reached. */
        uint16_t count;
        /** The code of the first glyph */
        uint8_t first;
        /** The glyph shown for a code the font does not hold, counted from the first glyph.
         * It is taken as it is: one past the font's glyphs is the program's to have there. */
        uint8_t fallback;
        /** A glyph's width in dots */
        uint8_t width;
        /** A glyph's height in dots */
        uint8_t height;
} hx_font_t;

/** The primary font: the one of the SDK's fonts that the program's Makefile names as
 * HX_FONT, or Unscii 8 by 8 when it names none. A call that takes a font takes this one for a
 * null pointer.
 *
 * The program's build defines it. A program that defines it too does not link. */
extern const hx_font_t * const hx_font;

#ifdef __cplusplus
}
#endif

#endif /* !HX_FONT_H */
