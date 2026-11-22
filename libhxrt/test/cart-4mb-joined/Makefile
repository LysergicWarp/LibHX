# The cart test with the 4 Mbyte RAM cartridge there and HX_CART_JOIN set, which changes
# nothing at that size
HX_SOURCES:= ../cart/main.c
HX_CART_JOIN:= 1
HX_TEST_CFLAGS:= -DEXPECT_ID=0x5C
HX_TEST_CORE_OPTIONS:= beetle_saturn_cart=extram4
include ../../../build/test.mk
