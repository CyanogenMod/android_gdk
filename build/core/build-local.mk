# TODO(Nowar): Let it more smart
GDK_PROJECT_ROOT := $(shell pwd)
GDK_PROJECT_OBJ := $(GDK_PROJECT_ROOT)/obj/llvm
GDK_PROJECT_OUT := $(GDK_PROJECT_ROOT)/res/raw
GDK_ROOT := $(GDK_PROJECT_ROOT)/../..

ifeq ($(NDK_ROOT),)
  $(info Android GDK: Must figure out NDK_ROOT location.)
  $(error Aborting.)
endif

include $(GDK_ROOT)/build/core/init.mk

include $(GDK_TOOLCHAIN_ROOT)/setup.mk

.PHONY: all \
        clean \
        bitcode_modules

ALL_BITCODE_MODULES :=

-include $(GDK_PROJECT_ROOT)/jni/Android-portable.mk

# The first rule
all: clean bitcode_modules

# TODO(Nowar): Do a real cleanup.
clean:
$(info rm -rf $(GDK_PROJECT_OBJ))
$(info rm -rf $(GDK_PROJECT_OUT)/*.bc)
$(info mkdir -p $(GDK_PROJECT_OBJ))
$(info mkdir -p $(GDK_PROJECT_OUT))

bitcode_modules: $(ALL_BITCODE_MODULES)

# Now all modules are in ALL_BITCODE_MODULES
# Take them out for each and build it
include $(GDK_ROOT)/build/core/build-modules.mk

# ############################################################################
#
# Trasnfer the control to NDK
#
# ############################################################################
#include $(NDK_ROOT)/build/core/build-local.mk
