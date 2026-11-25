# Builds the SDK's fonts into HX_OUT, a folder of the program that builds them:
#   HX_OUT/fonts.a                      a member a font
#   HX_OUT/fonts/primary-<font>.o       for each font, hx_font defined as that font. A
#                                       program's link takes the one its HX_FONT names.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk
include fonts.mk

HX_LIB_SOURCES:= $(addsuffix .c,$(HX_FONTS))

# The fonts' headers and the font type's
HX_LIB_INCLUDES:= -Iinclude -I../libhxrt/include

include ../build/library.mk

HX_FONT_PRIMARIES:= $(patsubst %,$(HX_LIB_OBJ)/primary-%.o,$(HX_FONTS))

all: $(HX_FONT_PRIMARIES)

# One source for all of them, built with the font's name and its header
$(HX_FONT_PRIMARIES): $(HX_LIB_OBJ)/primary-%.o: primary.c | $(HX_LIB_OBJ)
	$(HX_CC) $(HX_CFLAGS) $(HX_LIB_INCLUDES) -DHX_FONT_PRIMARY=hx_font_$* \
	    -include hx/font/$*.h -MMD -MP -c -o $@ $<

# A change to the list of fonts builds every object anew, as one to the Makefile does
$(HX_LIB_OBJS): fonts.mk
$(HX_FONT_PRIMARIES): Makefile fonts.mk ../build/fixed.mk ../build/flags.mk ../build/library.mk

-include $(HX_FONT_PRIMARIES:.o=.d)
