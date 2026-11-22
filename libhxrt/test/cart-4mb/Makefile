# The cart test with the 4 Mbyte RAM cartridge there: one run
HX_SOURCES:= ../cart/main.c
HX_TEST_CFLAGS:= -DEXPECT_ID=0x5C
HX_TEST_CORE_OPTIONS:= beetle_saturn_cart=extram4
include ../../../build/test.mk
