#include <hx/font.h>

/* The primary font's definition. Built once for each of the SDK's fonts, with HX_FONT_PRIMARY
 * as that font's name and the font's own header taken in ahead of this file: a program's
 * link takes the object of the font its Makefile names */
#ifndef HX_FONT_PRIMARY
#error "HX_FONT_PRIMARY is not defined: the name of the font that hx_font is"
#endif

const hx_font_t * const hx_font = &HX_FONT_PRIMARY;
