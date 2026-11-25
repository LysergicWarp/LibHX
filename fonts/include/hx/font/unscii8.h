#ifndef HX_FONT_UNSCII8_H
#define HX_FONT_UNSCII8_H

#include <hx/font.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @file
 * Unscii 8 by 8: a font of 96 glyphs, each 8 by 8 dots, from Viznut's Unscii
 * (https://viznut.fi/unscii/, the variant unscii-8), which is in the public domain.
 *
 * Codes 0x20 to 0x7E are ASCII's. Code 0x7F is a hollow square, Unscii's U+25A1, and is the
 * glyph shown for a code the font does not hold. */

/** Unscii 8 by 8 */
extern const hx_font_t hx_font_unscii8;

#ifdef __cplusplus
}
#endif

#endif /* !HX_FONT_UNSCII8_H */
