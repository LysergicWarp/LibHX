# hx_test_bands with 41 rows, the last one failing: every band green, and a red one below them
HX_SOURCES:= ../bands.c
HX_TEST_CFLAGS:= -DROWS=ppppppppppppppppppppppppppppppppppppppppF
include ../bands.mk
HX_TEST_SCREEN_WIDTH:= 320
HX_TEST_SCREEN_LINES:= 2:0000 \
    $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) $(PASSES) \
    $F \
    2:0000
HX_TEST_SCREEN_BORDER:= 0000
include ../../../build/test.mk
