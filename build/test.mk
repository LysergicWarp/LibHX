# A test disc: a program on the SDK that leaves a report in work RAM (libhxtest), and the disc
# around it. A test sits in a folder of its own, tests/<group>/<name> of the clone: the group
# is what it tests. Its Makefile sets its sources and includes this by its path from there:
#
#     HX_SOURCES:= main.c
#     include ../../../build/test.mk
#
# Tests of a group that share a source keep it in the group's folder, as ../main.c.
#
# It may set the link's settings (program.mk), and:
#   HX_TEST_CFLAGS         more for its C and its C++, as -D
#   HX_TEST_CORE_OPTIONS   the options of the emulator's core its run needs, each as
#                          <name>=<value>: the cartridge in the slot, for one
#   HX_TEST_SCREEN_LINES   the picture its run is to show, from the top: each <lines>:<colour>
#                          is that many lines of one colour. A colour is 4 hexadecimal digits,
#                          the 15 bits VDP2 has it in: 5 each of blue, green and red.
#   HX_TEST_SCREEN_PICTURE in place of HX_TEST_SCREEN_LINES, a file that holds the picture,
#                          for lines of more than one colour. A line of it is
#                              <lines> <pixels>:<colour> <pixels>:<colour> ...
#                          that many lines of the picture, each those runs of pixels from the
#                          left. The pixels of a line add up to the width. A number is 1 to
#                          9999. A # starts a comment.
#   HX_TEST_SCREEN_WIDTH   the picture's width in pixels, with either
#   HX_TEST_SCREEN_BORDER  the colour around the picture, with either
#
# make takes:
#   HX_OUT           the folder of the build. build, in the test's folder, when it is not given.
#   HX_BIOS          the BIOS file (disc.mk)
#   HX_TEST_REGION   the region of the console the disc is run on: na, eu or jp. na when it
#                    is not given. A test's sources see HX_TEST_PAL, 1 for eu and 0 otherwise.
#
# The disc is HX_OUT/<folder>.cue, the program HX_OUT/<folder>.elf, and HX_OUT/<folder>.nm
# lists the program's symbols, the report's among them, each with its address and size.
# HX_OUT/<folder>.options holds the core's options, one a line. HX_OUT/<folder>.screen holds
# the picture: the width and the border's colour, then lines as HX_TEST_SCREEN_PICTURE's file
# has them.
#
# A test is C23 or C++23 at -O2 with debug data and the SDK's warnings.
HX_ROOT:= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
HX_OUT?= build

HX_TEST:= $(notdir $(CURDIR))
HX_TEST_GROUP:= $(notdir $(patsubst %/,%,$(dir $(CURDIR))))
HX_PROGRAM:= $(HX_OUT)/$(HX_TEST)

HX_TEST_REGION?= na
ifeq ($(filter na eu jp,$(HX_TEST_REGION)),)
  $(error HX_TEST_REGION is $(HX_TEST_REGION). It is na, eu or jp)
endif
HX_TEST_PAL:= -DHX_TEST_PAL=$(if $(filter eu,$(HX_TEST_REGION)),1,0)

# The names the right sides use come with program.mk
HX_PROGRAM_CFLAGS= -std=c23 -O2 -g $(HX_WARNINGS) $(HX_C_WARNINGS) $(HX_TEST_CPPFLAGS) \
    $(HX_TEST_PAL) $(HX_TEST_CFLAGS)
HX_PROGRAM_CXXFLAGS= -std=c++23 -O2 -g $(HX_WARNINGS) $(HX_TEST_CPPFLAGS) $(HX_TEST_PAL) \
    $(HX_TEST_CFLAGS)
HX_PROGRAM_ASFLAGS= -Wa,--fatal-warnings
HX_PROGRAM_ARCHIVES= $(HX_TEST_ARCHIVES)

.PHONY: all

all:

include $(HX_ROOT)/build/program.mk

# One date for every test, so the same sources give the same disc
HX_DISC:= $(HX_OUT)/$(HX_TEST)
HX_DISC_FIRST:= $(HX_PROGRAM).bin
HX_IP_TITLE:= HX TEST $(HX_TEST_GROUP) $(HX_TEST)
HX_IP_DATE:= 20261120
include $(HX_ROOT)/build/disc.mk

$(foreach option,$(HX_TEST_CORE_OPTIONS),$(if $(findstring =,$(option)),, \
    $(error HX_TEST_CORE_OPTIONS holds $(option). An option is <name>=<value>)))
$(foreach option,$(HX_TEST_CORE_OPTIONS), \
    $(if $(findstring ',$(option))$(findstring ",$(option))$(findstring \,$(option)), \
        $(error HX_TEST_CORE_OPTIONS holds a quote or a backslash)))

HX_TEST_SCREEN_PICTURE:= $(strip $(HX_TEST_SCREEN_PICTURE))
ifneq ($(strip $(HX_TEST_SCREEN_LINES)),)
  ifneq ($(HX_TEST_SCREEN_PICTURE),)
    $(error HX_TEST_SCREEN_LINES and HX_TEST_SCREEN_PICTURE are both set. A picture is one or the other)
  endif
endif
ifeq ($(strip $(HX_TEST_SCREEN_LINES))$(HX_TEST_SCREEN_PICTURE),)
  ifneq ($(strip $(HX_TEST_SCREEN_WIDTH)$(HX_TEST_SCREEN_BORDER)),)
    $(error HX_TEST_SCREEN_WIDTH or HX_TEST_SCREEN_BORDER is set with no picture (HX_TEST_SCREEN_LINES or HX_TEST_SCREEN_PICTURE))
  endif
else
  ifeq ($(strip $(HX_TEST_SCREEN_WIDTH)),)
    $(error The picture is set with no HX_TEST_SCREEN_WIDTH (its width))
  endif
  ifeq ($(strip $(HX_TEST_SCREEN_BORDER)),)
    $(error The picture is set with no HX_TEST_SCREEN_BORDER (the colour around it))
  endif
endif
ifneq ($(words $(HX_TEST_SCREEN_PICTURE)),$(if $(HX_TEST_SCREEN_PICTURE),1,0))
  $(error HX_TEST_SCREEN_PICTURE is $(HX_TEST_SCREEN_PICTURE). It is one file)
endif
$(foreach run,$(HX_TEST_SCREEN_LINES),$(if $(filter 2,$(words $(subst :, ,$(run)))),, \
    $(error HX_TEST_SCREEN_LINES holds $(run). A run is <lines>:<colour>)))
$(foreach name,HX_TEST_SCREEN_LINES HX_TEST_SCREEN_PICTURE HX_TEST_SCREEN_WIDTH HX_TEST_SCREEN_BORDER, \
    $(if $(findstring ',$($(name)))$(findstring ",$($(name)))$(findstring \,$($(name))), \
        $(error $(name) holds a quote or a backslash)))

all: $(HX_DISC_IMAGE) $(HX_PROGRAM).nm $(HX_PROGRAM).options $(HX_PROGRAM).screen

$(HX_PROGRAM).nm: $(HX_PROGRAM).elf
	$(HX_NM) --numeric-sort --print-size --defined-only $< > $@.new
	mv $@.new $@

# A file with no line when the test names none. Written on every run, so options given on
# make's command line are the ones in it
$(HX_PROGRAM).options: hx-program-always
	@mkdir -p $(@D)
	@printf '%s\n' $(foreach option,$(HX_TEST_CORE_OPTIONS),'$(option)') | sed '/^$$/d' > $@.new
	@mv $@.new $@

# A file with no line when the test names no picture. Written on every run, as the options are.
# The first line is the width and the border's colour. Each other is a count of lines and
# their runs of pixels: a run of HX_TEST_SCREEN_LINES is the picture's width of its colour.
# A number is 1 to 9999. A byte order mark at the file's start is dropped
$(HX_PROGRAM).screen: hx-program-always $(HX_TEST_SCREEN_PICTURE)
	@mkdir -p $(@D)
	@{ printf '%s\n' \
	    $(if $(strip $(HX_TEST_SCREEN_LINES))$(HX_TEST_SCREEN_PICTURE),'$(strip $(HX_TEST_SCREEN_WIDTH)) $(strip $(HX_TEST_SCREEN_BORDER))') \
	    $(foreach run,$(HX_TEST_SCREEN_LINES),'$(word 1,$(subst :, ,$(run))) $(strip $(HX_TEST_SCREEN_WIDTH)):$(word 2,$(subst :, ,$(run)))'); \
	    $(if $(HX_TEST_SCREEN_PICTURE),sed -E -e '1s/^\xEF\xBB\xBF//' -e 's/#.*//' -e 's/[[:space:]]+/ /g' -e 's/^ //' -e 's/ $$//' -- '$(HX_TEST_SCREEN_PICTURE)';) \
	    } | sed '/^$$/d' > $@.new
	@if [ -s $@.new ] && ! head -n 1 $@.new | grep -Eq '^[1-9][0-9]{0,3} [0-9a-f]{4}$$'; then \
	    echo 'The picture has no width and border colour of 4 hexadecimal digits, in lower case:' >&2; \
	    head -n 1 $@.new >&2; \
	    rm $@.new; exit 1; fi
	@if [ -s $@.new ] && [ "$$(wc -l < $@.new)" -lt 2 ]; then \
	    echo 'The picture has no line.' >&2; \
	    rm $@.new; exit 1; fi
	@if sed 1d $@.new | grep -Evq '^[1-9][0-9]{0,3}( [1-9][0-9]{0,3}:[0-9a-f]{4})+$$'; then \
	    echo 'The picture holds a line that is no count of lines and runs of <pixels>:<colour>, each number 1 to 9999 and the colour 4 hexadecimal digits in lower case:' >&2; \
	    sed 1d $@.new | grep -Ev '^[1-9][0-9]{0,3}( [1-9][0-9]{0,3}:[0-9a-f]{4})+$$' >&2; \
	    rm $@.new; exit 1; fi
	@if ! awk 'NR == 1 { width = $$1; next } { pixels = 0; for (run = 2; run <= NF; run++) { split($$run, part, ":"); pixels += part[1] } if (pixels != width) { print > "/dev/stderr"; bad = 1 } } END { exit bad }' $@.new; then \
	    echo 'The pixels of those lines of the picture do not add up to its width.' >&2; \
	    rm $@.new; exit 1; fi
	@mv $@.new $@
