# Builds libhxrt.a into HX_OUT, a folder of the program that builds it.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

HX_LIB_SOURCES:= \
	start.sx

# The library's headers and the register maps
HX_LIB_INCLUDES:= -Iinclude -I../hw/include

include ../build/library.mk
