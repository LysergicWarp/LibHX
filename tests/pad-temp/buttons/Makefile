# hx_test_pad_temp: each button of the first port's pad on its own.
#
# A button is held for 12 frames, and the next follows 24 frames after it. The first is at
# frame 1000, which is past the program's start. In the test's order: Up, Down, Left, Right,
# Start, A, B, C, X, Y, Z, L, R, each as the button of the libretro joypad that Beetle Saturn
# takes for it.
HX_SOURCES:= main.c
HX_TEST_INPUTS:= \
    1000:0:4:1 1012:0:4:0 \
    1024:0:5:1 1036:0:5:0 \
    1048:0:6:1 1060:0:6:0 \
    1072:0:7:1 1084:0:7:0 \
    1096:0:3:1 1108:0:3:0 \
    1120:0:0:1 1132:0:0:0 \
    1144:0:8:1 1156:0:8:0 \
    1168:0:11:1 1180:0:11:0 \
    1192:0:1:1 1204:0:1:0 \
    1216:0:9:1 1228:0:9:0 \
    1240:0:10:1 1252:0:10:0 \
    1264:0:12:1 1276:0:12:0 \
    1288:0:13:1 1300:0:13:0
include ../../../build/test.mk
