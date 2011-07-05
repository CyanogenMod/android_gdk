# TODO(Nowar): Make below rules work.

#
# $1: c source file name
# $2: LOCAL_MODULE
#
define compile-c-to-bc
$(info $(BITCODE_CC) $(BITCODE_MODULE.$(2).PATH)/$(1)\
  $(BITCODE_MODULE.$(2).CFLAGS) $(BITCODE_CFLAGS)\
  -I$(BITCODE_MODULE.$(2).PATH)\
  -c -o $(GDK_PROJECT_OBJ)/$(2)/$(1:%.c=%.bc))
endef

#
# $1: c++ source file name
# $2: LOCAL_MODULE
#
define compile-cpp-to-bc
$(info $(BITCODE_CPP) $(BITCODE_MODULE.$(2).PATH)/$(1)\
  $(BITCODE_MODULE.$(2).CPPFLAGS) $(BITCODE_CPPFLAGS)\
  -I$(BITCODE_MODULE.$(2).PATH)\
  -c -o $(GDK_PROJECT_OBJ)/$(2)/$(1:%.cpp=%.bc))
endef

#
# $1: source file names
# $2: LOCAL_MODULE
#
define cmd-build-c-source
$(foreach gdk_source, $(1),\
  $(call compile-c-to-bc,$(gdk_source),$(2))\
)
endef

#
# $1: source file names
# $2: LOCAL_MODULE
#
define cmd-build-cpp-source
$(foreach gdk_source, $(1),\
  $(call compile-cpp-to-bc,$(gdk_source),$(2))\
)
endef

#
# $1: bc obj files name
# $2: LOCAL_MODULE
#
define cmd-link-bc
$(info $(BITCODE_LD) $(GDK_PROJECT_OBJ)/$(2)/*.bc -o $(GDK_PROJECT_OUT)/lib$(2).bc)
endef

$(foreach gdk_module, $(ALL_BITCODE_MODULES),\
  $(call cmd-build-c-source,$(filter %.c,$(BITCODE_MODULE.$(gdk_module).SRC_FILES)),$(gdk_module))\
  $(call cmd-build-cpp-source,$(filter %.cpp,$(BITCODE_MODULE.$(gdk_module).SRC_FILES)),$(gdk_module))\
  $(call cmd-link-bc,$(BITCODE_MODULE.$(gdk_module).SRC_FILES),$(gdk_module))\
)
