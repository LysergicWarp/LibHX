# Builds libhxmath.a and libhxmath_ref.a into HX_OUT, a folder of the program that builds them.
# libhxmath_ref.a holds the assembly functions in C, for tests alone. Link it ahead of libhxmath.a.
# HX_CC and HX_AR are the SH-2 compiler and archiver.
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (the folder the archives are built into))
endif

HX_CC?= sh2eb-elf-gcc
HX_AR?= sh2eb-elf-ar

SRCS:= \
	atan2.sx \
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
	tan_table.c \
	vec.c \
	vec3_cross.sx \
	vec3_normalize.sx

REF_SRCS:= \
	ref_atan2.c \
	ref_lookat.c \
	ref_mat.c \
	ref_sqrt.c \
	ref_vec3.c \
	ref_view.c

OBJS:= $(patsubst %,$(HX_OUT)/%.o,$(basename $(SRCS)))
REF_OBJS:= $(patsubst %,$(HX_OUT)/%.o,$(basename $(REF_SRCS)))

CFLAGS:= \
	-std=c11 \
	-m2 \
	-mb \
	-O2 \
	-ffreestanding \
	-fomit-frame-pointer \
	-ffunction-sections \
	-fdata-sections \
	-Wall \
	-Wextra \
	-Wshadow \
	-Iinclude

.PHONY: all

all: $(HX_OUT)/libhxmath.a $(HX_OUT)/libhxmath_ref.a

$(HX_OUT)/libhxmath.a: $(OBJS)
	rm -f $@
	$(HX_AR) rcs $@ $^

$(HX_OUT)/libhxmath_ref.a: $(REF_OBJS)
	rm -f $@
	$(HX_AR) rcs $@ $^

$(HX_OUT)/%.o: src/%.c | $(HX_OUT)
	$(HX_CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(HX_OUT)/%.o: src/%.sx | $(HX_OUT)
	$(HX_CC) -m2 -mb -MMD -MP -c -o $@ $<

$(HX_OUT)/%.o: ref/%.c | $(HX_OUT)
	$(HX_CC) $(CFLAGS) -Iref/include -Isrc -MMD -MP -c -o $@ $<

$(HX_OUT):
	mkdir -p $@

-include $(OBJS:.o=.d) $(REF_OBJS:.o=.d)
