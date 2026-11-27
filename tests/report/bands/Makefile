# hx_test_bands with twelve rows, four of them failing, and two notes among them: both
# colours, the gap after a fifth row, and no band for a note
HX_SOURCES:= ../bands.c
HX_TEST_CFLAGS:= -DROWS=nppFpppFnFpppF
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $P $L $P $L $F $L $P $L $P $G \
    $P $L $F $L $F $L $P $L $P $G \
    $P $L $F \
    159:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
