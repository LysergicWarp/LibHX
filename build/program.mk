# Builds a program against the SDK: its objects, the SDK's libraries, the link, and the file the
# BIOS loads. Needs GNU make 4.3 and the SH-2 compilers and objcopy.
#
# The program sets, before the include:
#   HX_ROOT      the LibHX clone
#   HX_OUT       a folder of its build
#   HX_PROGRAM   the program's path without an extension, for HX_PROGRAM.elf, HX_PROGRAM.map
#                and HX_PROGRAM.bin, the file the BIOS loads (disc.mk's HX_DISC_FIRST)
#   HX_SOURCES   its sources, .c, .cpp and .sx, by their paths from its Makefile's folder
# No path holds a space, and no value a quote or a backslash.
#
# It may set:
#   HX_PROGRAM_CFLAGS     what its C is compiled with: the level, the standard, warnings, its
#                         own include paths
#   HX_PROGRAM_CXXFLAGS   the same for its C++
#   HX_PROGRAM_ASFLAGS    the same for its assembly
#   HX_PROGRAM_ARCHIVES   archives of its own, linked ahead of the SDK's
#   HX_CC, HX_CXX, HX_AR, HX_OBJCOPY   the SH-2 tools, when they are not the ones on the path
#
# And the link's settings. Each reaches the link as a symbol, so the SDK's libraries are the
# same archives whatever a program picks:
#   HX_STACK           high, for a stack at the top of Work RAM-H, or low, for one below the
#                      program: from the program's first byte down to 0x06001000, 12 kbytes at
#                      the usual 1st Read Address, and none of the program's. high when it is
#                      not set.
#   HX_STACK_SIZE      the size of the stack at the top of Work RAM-H, a multiple of 4. 0x4000
#                      when it is not set.
#   HX_CART_JOIN       1 for startup to try the 1 Mbyte RAM cartridge's memory as one run,
#                      where it is two runs of 512 kbytes. Experimental: it holds on the
#                      cartridges whose first block shows again below the second, and the
#                      program finds two runs on the others. 0 when it is not set.
#   HX_IP_FIRST_READ   the address the BIOS loads the program to, as disc.mk has it
#
# The SDK fixes the SH-2, GCC's default calling convention, a section for each function and
# object, and no C library (HX_FIXED, fixed.mk). C++ is built without exceptions and run-time
# type information. The first use of a local static object takes no lock, and a destructor of
# a static object is not handed to a C++ runtime.
#
# Debug data names the program's folder as . and the clone as LibHX, so the same sources give
# the same HX_PROGRAM.elf wherever they are built.
ifeq ($(strip $(HX_ROOT)),)
  $(error Undefined HX_ROOT (the LibHX clone))
endif
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (a folder of the build))
endif
ifeq ($(strip $(HX_PROGRAM)),)
  $(error Undefined HX_PROGRAM (the program's path without an extension))
endif
ifeq ($(strip $(HX_SOURCES)),)
  $(error Undefined HX_SOURCES (the program's sources))
endif

include $(HX_ROOT)/build/fixed.mk
include $(HX_ROOT)/build/libs.mk

HX_STACK?= high
HX_CART_JOIN?= 0
HX_IP_FIRST_READ?= 0x06004000

ifeq ($(HX_STACK),high)
  HX_STACK_SIZE?= 0x4000
else ifeq ($(HX_STACK),low)
  ifneq ($(strip $(HX_STACK_SIZE)),)
    $(error HX_STACK_SIZE is set with HX_STACK low. The stack below the program has one size)
  endif
else
  $(error HX_STACK is $(HX_STACK). It is high or low)
endif

ifeq ($(HX_CART_JOIN),0)
else ifeq ($(HX_CART_JOIN),1)
else
  $(error HX_CART_JOIN is $(HX_CART_JOIN). It is 0 or 1)
endif

# What the objects are made with, and what the link is, beside the files themselves
HX_PROGRAM_FLAGS:= HX_CC HX_CXX HX_PROGRAM_CFLAGS HX_PROGRAM_CXXFLAGS HX_PROGRAM_ASFLAGS
HX_PROGRAM_SETTINGS:= HX_SOURCES HX_STACK HX_STACK_SIZE HX_CART_JOIN HX_IP_FIRST_READ \
    HX_PROGRAM_ARCHIVES HX_OBJCOPY

# A value reaches the shell between single quotes
$(foreach name,$(HX_PROGRAM_FLAGS) $(HX_PROGRAM_SETTINGS) HX_ROOT HX_OUT HX_PROGRAM HX_SOURCES, \
    $(if $(findstring ',$($(name)))$(findstring ",$($(name)))$(findstring \,$($(name))), \
        $(error $(name) holds a quote or a backslash)))

HX_CXX_FIXED:= \
	-fno-exceptions \
	-fno-rtti \
	-fno-threadsafe-statics \
	-fno-use-cxa-atexit

# The later of two that fit a path is the one taken: a program inside the clone is named by
# the clone. The compiler takes the folder of the build from PWD.
HX_PROGRAM_PATHMAP:= -ffile-prefix-map='$(CURDIR)'=. \
    -ffile-prefix-map='$(HX_ROOT_WHOLE)'=LibHX
export PWD:= $(CURDIR)

# The program's flags come first, so none of them undoes what the SDK fixes
HX_PROGRAM_COMPILE.c= $(HX_CC) $(HX_PROGRAM_CFLAGS) $(HX_CPPFLAGS) $(HX_FIXED) \
    $(HX_PROGRAM_PATHMAP)
HX_PROGRAM_COMPILE.cpp= $(HX_CXX) $(HX_PROGRAM_CXXFLAGS) $(HX_CPPFLAGS) $(HX_FIXED) \
    $(HX_CXX_FIXED) $(HX_PROGRAM_PATHMAP)
HX_PROGRAM_COMPILE.sx= $(HX_CC) $(HX_PROGRAM_ASFLAGS) $(HX_CPPFLAGS) -m2 -mb \
    $(HX_PROGRAM_PATHMAP)

$(foreach source,$(HX_SOURCES),$(if $(filter .c .cpp .sx,$(suffix $(source))),, \
    $(error HX_SOURCES holds $(source). A source is .c, .cpp or .sx)))

# The objects sit in a folder of the program's own, each under its source's path and whole
# name. A source above the Makefile's folder has "up" for each step there.
HX_PROGRAM_OBJ:= $(HX_OUT)/program
hx-program-object= $(HX_PROGRAM_OBJ)/$(subst ../,up/,$(1)).o
HX_PROGRAM_OBJS:= $(foreach source,$(HX_SOURCES),$(call hx-program-object,$(source)))

# A change to a makefile builds every object anew, as a changed flag does
HX_PROGRAM_MAKEFILES:= $(MAKEFILE_LIST)

# The program's first rule stays its default goal
HX_GOAL_BEFORE:= $(.DEFAULT_GOAL)

.PHONY: hx-program-always

$(HX_PROGRAM_OBJ):
	mkdir -p $@

# Written anew when a value is another than last time. So a value given on make's command line
# builds what it goes into.
$(HX_PROGRAM_OBJ)/flags: hx-program-always | $(HX_PROGRAM_OBJ)
	@printf '%s\n' $(foreach name,$(HX_PROGRAM_FLAGS),'$(name)=$($(name))') > $@.new
	@cmp -s $@.new $@ && rm $@.new || mv $@.new $@

$(HX_PROGRAM_OBJ)/settings: hx-program-always | $(HX_PROGRAM_OBJ)
	@printf '%s\n' $(foreach name,$(HX_PROGRAM_SETTINGS),'$(name)=$($(name))') > $@.new
	@cmp -s $@.new $@ && rm $@.new || mv $@.new $@

# The settings as the linker takes them
$(HX_PROGRAM_OBJ)/settings.ld: $(HX_PROGRAM_OBJ)/settings $(HX_PROGRAM_MAKEFILES)
	printf '%s\n' \
	    'hx_load = $(HX_IP_FIRST_READ);' \
	    'hx_stack_low = $(if $(filter low,$(HX_STACK)),1,0);' \
	    'hx_stack_size = $(or $(strip $(HX_STACK_SIZE)),0);' \
	    'hx_cart_join = $(HX_CART_JOIN);' > $@

define HX_PROGRAM_RULE
$(call hx-program-object,$(1)): $(1) $(HX_PROGRAM_OBJ)/flags $(HX_PROGRAM_MAKEFILES)
	@mkdir -p $$(@D)
	$$(HX_PROGRAM_COMPILE$(suffix $(1))) -MMD -MP -c -o $$@ $$<
endef

$(foreach source,$(HX_SOURCES),$(eval $(call HX_PROGRAM_RULE,$(source))))

# Every section is placed by the linker script, and one it does not name stops the link. What
# nothing reaches from startup is dropped. libgcc is the compiler's own, for what it calls.
# The linker's warnings stop the link, but for the one of memory that is written and run: all
# of work RAM is.
$(HX_PROGRAM).elf: $(HX_PROGRAM_OBJS) $(HX_PROGRAM_ARCHIVES) $(HX_ARCHIVES) \
    $(HX_ROOT)/libhxrt/program.ld $(HX_PROGRAM_OBJ)/settings.ld $(HX_PROGRAM_OBJ)/flags
	@mkdir -p $(@D)
	$(HX_CC) -m2 -mb -nostdlib -Wl,--gc-sections -Wl,--fatal-warnings \
	    -Wl,--no-warn-rwx-segments -Wl,--orphan-handling=error \
	    -Wl,-T,$(HX_PROGRAM_OBJ)/settings.ld -Wl,-T,$(HX_ROOT)/libhxrt/program.ld \
	    -Wl,-Map=$(HX_PROGRAM).map -o $@ \
	    $(HX_PROGRAM_OBJS) $(HX_PROGRAM_ARCHIVES) $(HX_LDLIBS) -lgcc

# The loaded sections' bytes, from the 1st Read Address on
$(HX_PROGRAM).bin: $(HX_PROGRAM).elf $(HX_PROGRAM_OBJ)/settings
	$(HX_OBJCOPY) -O binary $< $@

-include $(HX_PROGRAM_OBJS:.o=.d)

.DEFAULT_GOAL:= $(HX_GOAL_BEFORE)
