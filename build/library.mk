# The rules of a library's Makefile: its archive built into HX_OUT, a folder of the program that
# builds it, and make headers, which checks each public header alone and takes no HX_OUT.
#
# A library's Makefile includes flags.mk, sets these, and includes this:
#   HX_LIB_SOURCES       the library's sources, in src/
#   HX_LIB_REF_SOURCES   its references' sources, in ref/, when it has them: its assembly
#                        functions in C, for tests alone. They build with ref/include and src on
#                        their include path, into an archive of their own.
#   HX_LIB_INCLUDES      the include paths of both
#
# The archives are HX_OUT/<folder>.a and HX_OUT/<folder>_ref.a, by the library's folder name.
ifneq ($(MAKECMDGOALS),headers)
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (the folder the archives are built into))
endif
endif

# The objects sit in a folder of the library's own, beside the archives
HX_LIB_OBJ:= $(HX_OUT)/$(HX_LIBRARY)
HX_LIB_OBJS:= $(patsubst %,$(HX_LIB_OBJ)/%.o,$(basename $(HX_LIB_SOURCES)))
HX_LIB_REF_OBJS:= $(patsubst %,$(HX_LIB_OBJ)/%.o,$(basename $(HX_LIB_REF_SOURCES)))

HX_LIB_ARCHIVE:= $(HX_OUT)/$(HX_LIBRARY).a
HX_LIB_REF_ARCHIVE:= $(if $(HX_LIB_REF_SOURCES),$(HX_OUT)/$(HX_LIBRARY)_ref.a)

.PHONY: all

all: $(HX_LIB_ARCHIVE) $(HX_LIB_REF_ARCHIVE)

$(HX_LIB_ARCHIVE): $(HX_LIB_OBJS)
	rm -f $@
	$(HX_AR) $(HX_ARFLAGS) $@ $^

ifneq ($(HX_LIB_REF_ARCHIVE),)
$(HX_LIB_REF_ARCHIVE): $(HX_LIB_REF_OBJS)
	rm -f $@
	$(HX_AR) $(HX_ARFLAGS) $@ $^
endif

# A change to the library's Makefile or to the SDK's flags and rules builds every object anew
$(HX_LIB_OBJS) $(HX_LIB_REF_OBJS): Makefile ../build/fixed.mk ../build/flags.mk ../build/library.mk

$(HX_LIB_OBJ)/%.o: src/%.c | $(HX_LIB_OBJ)
	$(HX_CC) $(HX_CFLAGS) $(HX_LIB_INCLUDES) -MMD -MP -c -o $@ $<

$(HX_LIB_OBJ)/%.o: src/%.sx | $(HX_LIB_OBJ)
	$(HX_CC) $(HX_ASFLAGS) $(HX_LIB_INCLUDES) -MMD -MP -c -o $@ $<

$(HX_LIB_OBJ)/%.o: ref/%.c | $(HX_LIB_OBJ)
	$(HX_CC) $(HX_CFLAGS) $(HX_LIB_INCLUDES) -Iref/include -Isrc -MMD -MP -c -o $@ $<

$(HX_LIB_OBJ):
	mkdir -p $@

# A source that is gone and that a dependency file in HX_OUT still names: its object is made anew
# from what took its place
src/%.c: ;
src/%.sx: ;
ref/%.c: ;

-include $(HX_LIB_OBJS:.o=.d) $(HX_LIB_REF_OBJS:.o=.d)

HX_HEADERS:= $(shell find include -name '*.h' | LC_ALL=C sort)
HX_HEADER_FLAGS:= $(HX_LIB_INCLUDES)
include ../build/headers.mk
