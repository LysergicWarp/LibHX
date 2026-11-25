# Builds libhx2d.a into HX_OUT, a folder of the program that builds it.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

HX_LIB_SOURCES:= \
	text.c

# The library's headers, lib315's and the runtime's, and the register maps
HX_LIB_INCLUDES:= -Iinclude -I../lib315/include -I../libhxrt/include -I../hw/include

include ../build/library.mk
