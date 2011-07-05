# TODO(Nowar): Make codes below work.

#
# $1: c source file name
# $2: bc file name
# $3: LOCAL_MODULE
#
define compile-c-to-bc
$(info $(BITCODE_CC) $(BITCODE_MODULE.$(3).PATH)/$(1) $(BITCODE_MODULE.$(3).CFLAGS) -o $(GDK_PROJECT_OBJ)/$(2))
endef

#
# $1: c++ source file name
# $2: bc file name
# $3: LOCAL_MODULE
#
define compile-cpp-to-bc
$(info $(BITCODE_CPP) $(BITCODE_MODULE,$(3).PATH)/$(1) $(BITCODE_MODULE.$(3).CPPFLAGS) -o $(GDK_PROJECT_OBJ)/$(2))
endef

#
# $1: source file names
# $2: LOCAL_MODULE
#
# TODO(Nowar): How could we separate C++ from C source?
# FIXME(Nowar): Handle C++ case
define cmd-build-source
$(foreach gdk_source, $(1),\
  $(call compile-c-to-bc,$(gdk_source),$(gdk_source:%.c=%.bc),$(2))\
)
endef

#
# $1: bc obj files name
# $2: LOCAL_MODULE
#
define cmd-link-bc
$(info $(BITCODE_LD) $(patsubst %.c,$(GDK_PROJECT_OBJ)/%.bc,$(1)) -o $(GDK_PROJECT_OUT)/lib$(2).bc)
endef

$(foreach gdk_module, $(ALL_BITCODE_MODULES),\
  $(call cmd-build-source,$(BITCODE_MODULE.$(gdk_module).SRC_FILES),$(gdk_module))\
  $(call cmd-link-bc,$(BITCODE_MODULE.$(gdk_module).SRC_FILES),$(gdk_module))\
)
