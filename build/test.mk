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
#
# make takes:
#   HX_OUT    the folder of the build. build, in the test's folder, when it is not given.
#   HX_BIOS   the BIOS file (disc.mk)
#
# The disc is HX_OUT/<folder>.cue, the program HX_OUT/<folder>.elf, and HX_OUT/<folder>.nm
# lists the program's symbols, the report's among them, each with its address and size.
# HX_OUT/<folder>.options holds the core's options, one a line.
#
# A test is C23 or C++23 at -O2 with debug data and the SDK's warnings.
HX_ROOT:= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
HX_OUT?= build

HX_TEST:= $(notdir $(CURDIR))
HX_TEST_GROUP:= $(notdir $(patsubst %/,%,$(dir $(CURDIR))))
HX_PROGRAM:= $(HX_OUT)/$(HX_TEST)

# The names the right sides use come with program.mk
HX_PROGRAM_CFLAGS= -std=c23 -O2 -g $(HX_WARNINGS) $(HX_C_WARNINGS) $(HX_TEST_CPPFLAGS) \
    $(HX_TEST_CFLAGS)
HX_PROGRAM_CXXFLAGS= -std=c++23 -O2 -g $(HX_WARNINGS) $(HX_TEST_CPPFLAGS) $(HX_TEST_CFLAGS)
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

all: $(HX_DISC_IMAGE) $(HX_PROGRAM).nm $(HX_PROGRAM).options

$(HX_PROGRAM).nm: $(HX_PROGRAM).elf
	$(HX_NM) --numeric-sort --print-size --defined-only $< > $@.new
	mv $@.new $@

# A file with no line when the test names none. Written on every run, so options given on
# make's command line are the ones in it
$(HX_PROGRAM).options: hx-program-always
	@mkdir -p $(@D)
	@printf '%s\n' $(foreach option,$(HX_TEST_CORE_OPTIONS),'$(option)') | sed '/^$$/d' > $@.new
	@mv $@.new $@
