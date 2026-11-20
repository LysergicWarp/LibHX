# Builds libhxtest.a into HX_OUT, a folder of the test disc that builds it: the report a test
# disc leaves in work RAM. For tests alone.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

HX_LIB_SOURCES:= \
	report.c

HX_LIB_INCLUDES:= -Iinclude

include ../build/library.mk
