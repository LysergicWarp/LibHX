# hx_test_show with twelve rows, four of them failing: both colours, and the gap after a fifth
# row
HX_SOURCES:= ../main.c
HX_TEST_CFLAGS:= -DROWS=ppFpppFFpppF
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $P $L $P $L $F $L $P $L $P $G \
    $P $L $F $L $F $L $P $L $P $G \
    $P $L $F \
    159:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
