# What the SDK's libraries and a program's build share: the tools, the flags that fix how
# objects fit together, and the SDK's warnings.
#
# HX_CC, HX_CXX, HX_AR, HX_OBJCOPY and HX_NM are the SH-2 compilers, archiver, objcopy and nm.
HX_CC?= sh2eb-elf-gcc
HX_CXX?= sh2eb-elf-g++
HX_AR?= sh2eb-elf-ar
HX_OBJCOPY?= sh2eb-elf-objcopy
HX_NM?= sh2eb-elf-nm

# The SH-2, big-endian, GCC's default calling convention and no C library. Each function and
# object gets a section of its own, so a link can drop what a program does not use. Address 0 is
# the BIOS ROM and reads without a fault, so a check for a null pointer is never taken as proven
# by an access through that pointer.
HX_FIXED:= \
	-m2 \
	-mb \
	-ffreestanding \
	-ffunction-sections \
	-fdata-sections \
	-fno-delete-null-pointer-checks

# The warnings of the SDK's own sources, all of them errors. A program takes them or leaves
# them.
HX_WARNINGS:= \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wundef \
	-Wvla \
	-Wdouble-promotion \
	-Werror

HX_C_WARNINGS:= \
	-Wstrict-prototypes \
	-Wmissing-prototypes
