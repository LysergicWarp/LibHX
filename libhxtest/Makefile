# Builds libhxtest.a into HX_OUT, a folder of the test disc that builds it: the report a test
# disc leaves in work RAM, the report on screen, and a pad read for the time being. For tests
# alone.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

HX_LIB_SOURCES:= \
	bands.c \
	pad_temp.c \
	report.c \
	show.c

# The library's headers, the text calls', lib315's and the runtime's, and the register maps
HX_LIB_INCLUDES:= -Iinclude -I../libhx2d/include -I../lib315/include -I../libhxrt/include \
    -I../hw/include

include ../build/library.mk
