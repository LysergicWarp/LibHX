# make headers: each public header checked alone, as C11, as C23 and as C++23, with the SDK's
# warnings and those of strict ISO as errors. C11 is the oldest C a program is written in. A
# header has to take in all it needs, and what one of the three refuses does not pass.
#
# The compilers stop after their checks. Nothing is assembled, linked or written, so a name's
# linkage in C++ is not checked.
#
# A library's Makefile includes this after flags.mk and its own first rule.
#   HX_HEADERS        the headers
#   HX_HEADER_FLAGS   the include paths they need
HX_HEADER_CHECK= $(HX_FIXED) $(HX_WARNINGS) -Wpedantic $(HX_HEADER_FLAGS) -fsyntax-only

HX_HEADERS_C11:= $(addsuffix .c11,$(HX_HEADERS))
HX_HEADERS_C23:= $(addsuffix .c23,$(HX_HEADERS))
HX_HEADERS_CXX23:= $(addsuffix .c++23,$(HX_HEADERS))

.PHONY: headers $(HX_HEADERS_C11) $(HX_HEADERS_C23) $(HX_HEADERS_CXX23)

headers: $(HX_HEADERS_C11) $(HX_HEADERS_C23) $(HX_HEADERS_CXX23)

$(HX_HEADERS_C11): %.c11: %
	$(HX_CC) -x c -std=c11 $(HX_C_WARNINGS) $(HX_HEADER_CHECK) $<

$(HX_HEADERS_C23): %.c23: %
	$(HX_CC) -x c -std=c23 $(HX_C_WARNINGS) $(HX_HEADER_CHECK) $<

$(HX_HEADERS_CXX23): %.c++23: %
	$(HX_CXX) -x c++ -std=c++23 $(HX_HEADER_CHECK) $<
