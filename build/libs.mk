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
# And for a test disc, the report it leaves:
#   HX_TEST_CPPFLAGS, HX_TEST_ARCHIVES
ifeq ($(strip $(HX_ROOT)),)
  $(error Undefined HX_ROOT (the LibHX clone))
endif
ifeq ($(strip $(HX_OUT)),)
  $(error Undefined HX_OUT (a folder of the build, for the SDK's objects and archives))
endif

# In link order: a library ahead of the ones it uses
HX_LIBRARIES:= 315 hxmath hxrt

# The libraries that have references
HX_REF_LIBRARIES:= hxmath

# The headers by the clone's whole path, so a path map names them the same however HX_ROOT
# is spelled
HX_ROOT_WHOLE:= $(abspath $(HX_ROOT))

HX_CPPFLAGS:= -I$(HX_ROOT_WHOLE)/hw/include \
    $(foreach library,$(HX_LIBRARIES),-I$(HX_ROOT_WHOLE)/lib$(library)/include)
HX_ARCHIVES:= $(foreach library,$(HX_LIBRARIES),$(HX_OUT)/lib$(library).a)
HX_LDLIBS:= -L$(HX_OUT) $(addprefix -l,$(HX_LIBRARIES))

HX_REF_CPPFLAGS:= \
    $(foreach library,$(HX_REF_LIBRARIES),-I$(HX_ROOT_WHOLE)/lib$(library)/ref/include)
HX_REF_ARCHIVES:= $(foreach library,$(HX_REF_LIBRARIES),$(HX_OUT)/lib$(library)_ref.a)
HX_REF_LDLIBS:= -L$(HX_OUT) $(patsubst %,-l%_ref,$(HX_REF_LIBRARIES))

HX_TEST_CPPFLAGS:= -I$(HX_ROOT_WHOLE)/libhxtest/include
HX_TEST_ARCHIVES:= $(HX_OUT)/libhxtest.a

# A library's own make gives all its archives in one run. It decides what is out of date, so it
# runs on every build. The program's first rule stays its default goal.
HX_GOAL_BEFORE:= $(.DEFAULT_GOAL)

.PHONY: hx-libraries

define HX_LIBRARY_RULE
$(HX_OUT)/lib$(1).a $(if $(filter $(1),$(HX_REF_LIBRARIES)),$(HX_OUT)/lib$(1)_ref.a) &: hx-libraries
	$$(MAKE) --no-print-directory -C $(HX_ROOT)/lib$(1) HX_OUT=$(abspath $(HX_OUT)) \
	    $$(if $$(HX_CC),HX_CC='$$(HX_CC)') $$(if $$(HX_AR),HX_AR='$$(HX_AR)')
endef

$(foreach library,$(HX_LIBRARIES) hxtest,$(eval $(call HX_LIBRARY_RULE,$(library))))

.DEFAULT_GOAL:= $(HX_GOAL_BEFORE)
