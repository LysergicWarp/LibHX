# The cart test with a backup memory cartridge there: its ID, and no memory found
HX_SOURCES:= ../cart/main.c
HX_TEST_CFLAGS:= -DEXPECT_ID=0x21
HX_TEST_CORE_OPTIONS:= beetle_saturn_cart=backup
include ../../../build/test.mk
