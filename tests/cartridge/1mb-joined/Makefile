# The cartridge test with the 1 Mbyte RAM cartridge there and HX_CART_JOIN set: one run
HX_SOURCES:= ../main.c
HX_CART_JOIN:= 1
HX_TEST_CFLAGS:= -DEXPECT_ID=0x5A -DJOINED
HX_TEST_CORE_OPTIONS:= beetle_saturn_cart=extram1
include ../../../build/test.mk
