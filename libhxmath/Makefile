# Builds libhxmath.a and libhxmath_ref.a into HX_OUT, a folder of the program that builds them.
# libhxmath_ref.a holds the assembly functions in C, for tests alone. Link it ahead of libhxmath.a.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

ifneq ($(MAKECMDGOALS),headers)
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (the folder the archives are built into))
endif
endif

SRCS:= \
	atan2.c \
	atan2_fast.sx \
	atan_table.c \
	layout.c \
	mat.c \
	mat_batch.sx \
	mat_invert.sx \
	mat_lookat.sx \
	mat_mul.sx \
	mat_rotate.sx \
	mat_transform.sx \
	rsqrt.sx \
	rsqrt_table.c \
	sincos_table.c \
	sqrt.sx \
	sqrt_table.c \
	tan.c \
	tan_table.c \
	vec.c \
	vec3_cross.sx \
	vec3_normalize.sx

REF_SRCS:= \
	ref_atan2_fast.c \
	ref_lookat.c \
	ref_mat.c \
	ref_sqrt.c \
	ref_vec3.c \
	ref_view.c

# The library's headers and the register maps
INCLUDES:= -Iinclude -I../hw/include

# The objects sit in a folder of the library's own, beside the archives
OBJ:= $(HX_OUT)/$(HX_LIBRARY)
OBJS:= $(patsubst %,$(OBJ)/%.o,$(basename $(SRCS)))
REF_OBJS:= $(patsubst %,$(OBJ)/%.o,$(basename $(REF_SRCS)))

.PHONY: all

all: $(HX_OUT)/libhxmath.a $(HX_OUT)/libhxmath_ref.a

$(HX_OUT)/libhxmath.a: $(OBJS)
	rm -f $@
	$(HX_AR) $(HX_ARFLAGS) $@ $^

$(HX_OUT)/libhxmath_ref.a: $(REF_OBJS)
	rm -f $@
	$(HX_AR) $(HX_ARFLAGS) $@ $^

# A change to this file or to the SDK's flags builds every object anew
$(OBJS) $(REF_OBJS): Makefile ../build/flags.mk

$(OBJ)/%.o: src/%.c | $(OBJ)
	$(HX_CC) $(HX_CFLAGS) $(INCLUDES) -MMD -MP -c -o $@ $<

$(OBJ)/%.o: src/%.sx | $(OBJ)
	$(HX_CC) $(HX_ASFLAGS) $(INCLUDES) -MMD -MP -c -o $@ $<

$(OBJ)/%.o: ref/%.c | $(OBJ)
	$(HX_CC) $(HX_CFLAGS) $(INCLUDES) -Iref/include -Isrc -MMD -MP -c -o $@ $<

$(OBJ):
	mkdir -p $@

# A source that is gone and that a dependency file in HX_OUT still names: its object is made anew
# from what took its place
src/%.c: ;
src/%.sx: ;
ref/%.c: ;

-include $(OBJS:.o=.d) $(REF_OBJS:.o=.d)

HX_HEADERS:= $(shell find include -name '*.h' | LC_ALL=C sort)
HX_HEADER_FLAGS:= $(INCLUDES)
include ../build/headers.mk
