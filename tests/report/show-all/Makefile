# hx_test_show with 40 rows, the last one failing: every band, and none below them
HX_SOURCES:= ../main.c
HX_TEST_CFLAGS:= -DROWS=pppppppppppppppppppppppppppppppppppppppF
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) \
    $P $L $P $L $P $L $P $L $F \
    9:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
