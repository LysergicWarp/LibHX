# The flags of the SDK's own libraries. A library's Makefile includes this from its own folder,
# one below the clone's root.
#
# The header check alone uses HX_CXX.
include ../build/fixed.mk

# The rules a Makefile gives are all there are
MAKEFLAGS+= --no-builtin-rules
.SUFFIXES:

# The library's folder name
HX_LIBRARY:= $(lastword $(subst /, ,$(CURDIR)))

# The same source gives the same archives wherever it is built. Debug data from the compiler names
# the clone as LibHX, and a source file relative to its library's folder. The compiler takes the
# folder of the build from PWD, which a build entered through a link has as the link.
HX_PATHMAP:= -ffile-prefix-map='$(abspath ..)'=LibHX
export PWD:= $(CURDIR)

# The names GCC makes up for an object are seeded from the source's name, not at random. They
# show in objects built for link-time optimisation.
HX_SEED= -frandom-seed=$(HX_LIBRARY)/$<

# The SDK's own C is C23, at one optimisation level whatever the program's
HX_CFLAGS= \
	-std=c23 \
	-O2 \
	$(HX_FIXED) \
	$(HX_WARNINGS) \
	$(HX_C_WARNINGS) \
	$(HX_PATHMAP) \
	$(HX_SEED)

# Assembly goes through the compiler for its preprocessor
HX_ASFLAGS:= \
	-m2 \
	-mb \
	-Wa,--fatal-warnings \
	$(HX_PATHMAP)

# D: no time, owner or mode of a member in the archive
HX_ARFLAGS:= rcsD
