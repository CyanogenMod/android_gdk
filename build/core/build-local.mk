# TODO(Nowar): Let it more smart
GDK_PROJECT_ROOT := $(shell pwd)
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

# The first rule
all: bitcode_modules

clean:
	rm -rf $(LOCAL_PATH)/obj/llvm/
	rm -rf $(LOCAL_PATH)/res/raw/*.bc

include $(GDK_PROJECT_ROOT)/jni/Android-portable.mk

bitcode_modules: clean $(ALL_BITCODE_MODULES)

# Now all modules are in ALL_BITCODE_MODULES
# Take them out for each and build it
include $(GDK_ROOT)/build/core/build-modules.mk

# ############################################################################
#
# Trasnfer the control to NDK
#
# ############################################################################
include $(NDK_ROOT)/build/core/build-local.mk
