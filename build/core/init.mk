# We need borrow definitions.mk from NDK due to some predefines.
# Like: $(call my-dir)
include $(NDK_ROOT)/build/core/definitions.mk

GDK_PLATFORM_ROOT := $(GDK_ROOT)/platforms/android-portable/arch-llvm
GDK_TOOLCHAIN_ROOT := $(GDK_ROOT)/toolchains/llvm

CLEAR_VARS := $(GDK_ROOT)/build/core/clear-vars.mk
BUILD_BITCODE := $(GDK_ROOT)/build/core/build-bitcode.mk
