# Builds the SDK's libraries into a folder of the program that includes this. The clone is left
# as it is. Needs GNU make 4.3.
#
# The program sets, before the include:
#   HX_ROOT   the LibHX clone
#   HX_OUT    a folder of its build, for the SDK's objects and archives
#   HX_CC, HX_AR   the SH-2 compiler and archiver, when they are not the ones on the path
# HX_ROOT and HX_OUT are used as they are, so neither holds a space.
#
# The program gets:
#   HX_CPPFLAGS   the include paths of the SDK's headers
#   HX_ARCHIVES   the archives, for its link to depend on
#   HX_LDLIBS     the archives as the linker takes them, in link order
# And for a test that links libhxmath's references, each ahead of its counterpart above:
#   HX_REF_CPPFLAGS, HX_REF_ARCHIVES, HX_REF_LDLIBS
ifeq ($(strip $(HX_ROOT)),)
  $(error Undefined HX_ROOT (the LibHX clone))
endif
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (a folder of the build, for the SDK's objects and archives))
endif

# In link order: a library ahead of the ones it uses
HX_LIBRARIES:= hxmath

HX_CPPFLAGS:= $(foreach library,$(HX_LIBRARIES),-I$(HX_ROOT)/lib$(library)/include)
HX_ARCHIVES:= $(foreach library,$(HX_LIBRARIES),$(HX_OUT)/lib$(library).a)
HX_LDLIBS:= -L$(HX_OUT) $(addprefix -l,$(HX_LIBRARIES))

HX_REF_CPPFLAGS:= -I$(HX_ROOT)/libhxmath/ref/include
HX_REF_ARCHIVES:= $(HX_OUT)/libhxmath_ref.a
HX_REF_LDLIBS:= -L$(HX_OUT) -lhxmath_ref

# A library's own make gives all its archives in one run. It decides what is out of date, so it
# runs on every build. The program's first rule stays its default goal.
HX_GOAL_BEFORE:= $(.DEFAULT_GOAL)

.PHONY: hx-libraries

$(HX_OUT)/libhxmath.a $(HX_OUT)/libhxmath_ref.a &: hx-libraries
	$(MAKE) --no-print-directory -C $(HX_ROOT)/libhxmath HX_OUT=$(abspath $(HX_OUT)) \
	    $(if $(HX_CC),HX_CC='$(HX_CC)') $(if $(HX_AR),HX_AR='$(HX_AR)')

.DEFAULT_GOAL:= $(HX_GOAL_BEFORE)
