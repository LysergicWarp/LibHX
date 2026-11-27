# hx_test_show with 26 rows that fail and two that pass: the 26 that fail, as many as are
# shown, and the count of the two not shown in white, no row that fails among them
HX_SOURCES:= ../text.c
HX_FONT:= unscii8
HX_TEST_CFLAGS:= -DROWS=FFFFFFFFFFFFFFFFFFFFFFFFFFpp
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_PICTURE:= picture.txt
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
