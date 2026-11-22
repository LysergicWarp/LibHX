# hx_test_show with 43 rows, the last with a band failing and the three past it passing: every
# band, and the white one below them
HX_SOURCES:= ../main.c
HX_TEST_CFLAGS:= -DROWS=pppppppppppppppppppppppppppppppppppppppFppp
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) \
    $P $L $P $L $P $L $P $L $F $G \
    $M \
    2:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
