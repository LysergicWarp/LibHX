# The register maps: headers alone, nothing to build.
# make headers checks each map alone, as C11, C23 and C++23, and as the assembler takes it.
include ../build/flags.mk

HX_HEADERS:= $(shell find include -name '*.h' | LC_ALL=C sort)
HX_HEADER_FLAGS:= -Iinclude
include ../build/headers.mk

# To the assembler a map is its numbers and nothing else. Every macro the assembler is left with
# is assembled as a long, so one that is no number to it stops here
HX_HEADERS_AS:= $(addsuffix .as,$(HX_HEADERS))

.PHONY: $(HX_HEADERS_AS)

headers: $(HX_HEADERS_AS)

$(HX_HEADERS_AS): %.as: %
	$(HX_CC) -x assembler-with-cpp $(HX_HEADER_FLAGS) -E -dM $< | \
	    sed -n 's/^#define \([A-Z][A-Z0-9_]*\) ..*/        .long \1/p' | \
	    $(HX_CC) -x assembler-with-cpp $(HX_ASFLAGS) $(HX_HEADER_FLAGS) -include $< -c -o /dev/null -
