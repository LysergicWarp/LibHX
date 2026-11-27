# hx_test_bands with 41 rows that pass and a full report: every band green, and a red one
# below them where a white one would say more rows
HX_SOURCES:= ../bands.c
HX_TEST_CFLAGS:= -DROWS=ppppppppppppppppppppppppppppppppppppppppp -DFULL
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) \
    $F \
    2:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
