# ############################################################################
#
# Read all toolchain-specific configuration files.
#
# ############################################################################
GDK_PLATFORM_ROOT := $(GDK_ROOT)/platforms/android-portable/arch-llvm
GDK_TOOLCHAIN_ROOT := $(GDK_ROOT)/toolchains/llvm

BUILD_BITCODE := $(GDK_ROOT)/build/core/build-bitcode.mk

parent-dir = $(patsubst %/,%,%(dir $1))
my-dir = $(call parent-dir,$(lastword $(MAKEFILE_LIST)))

include $(GDK_TOOLCHAIN_ROOT)/setup.mk
