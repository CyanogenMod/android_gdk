GDK_PROJECT_PATH := $(shell pwd)
GDK_ROOT := $(GDK_PROJECT_PATH)/../..

include $(GDK_ROOT)/build/core/init.mk

include $(GDK_PROJECT_PATH)/jni/Android-portable.mk

include $(GDK_ROOT)/build/core/build-bitcode.mk

ifeq ($(NDK_ROOT),)
  $(info Android GDK: Must figure out NDK_ROOT location.)
  $(error Aborting.)
endif

include $(NDK_ROOT)/build/core/build-local.mk
