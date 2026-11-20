# Builds libhxmath.a and libhxmath_ref.a into HX_OUT, a folder of the program that builds them.
# libhxmath_ref.a holds the assembly functions in C, for tests alone. Link it ahead of libhxmath.a.
# make headers checks each public header alone, and takes no HX_OUT.
include ../build/flags.mk

HX_LIB_SOURCES:= \
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

HX_LIB_REF_SOURCES:= \
	ref_atan2_fast.c \
	ref_lookat.c \
	ref_mat.c \
	ref_sqrt.c \
	ref_vec3.c \
	ref_view.c

# The library's headers and the register maps
HX_LIB_INCLUDES:= -Iinclude -I../hw/include

include ../build/library.mk
