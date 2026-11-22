# hx_test_show with the test's own report, closed: seven rows that pass
HX_SOURCES:= ../main.c
HX_TEST_CFLAGS:= -DOWN=7
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $(PASSES) \
    $P $L $P \
    186:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
