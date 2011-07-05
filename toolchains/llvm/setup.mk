GDK_TOOLCHAIN_PREFIX := $(GDK_TOOLCHAIN_ROOT)/llvm/prebuilt/

# FIXME(Nowar): In future, we use llvm-ndk-cc and llvm-ndk-link.
#BITCODE_CC       := $(GDK_TOOLCHAIN_PREFIX)llvm-ndk-cc
#BITCODE_CFLAGS   :=
#
#BITCODE_CPP      := $(GDK_TOOLCHAIN_PREFIX)llvm-ndk-cc
#BITCODE_CPPFLAGS := $(BITCODE_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus
#
#BITCODE_LD       := $(GDK_TOOLCHAIN_PREFIX)llvm-ndk-link
#BITCODE_LDFLAGS  :=

BITCODE_CC := clang
BITCODE_CFLAGS := -ccc-host-triple armv7-none-linux-gnueabi -emit-llvm
BITCODE_CPP := clang++
BITCODE_CPPFLAGS := $(BITCODE_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus
BITCODE_LD := llvm-link
BITCODE_LDFLAGS :=

BITCODE_C_INCLUDES := \
  $(GDK_PLATFORM_ROOT)/usr/include

#define cmd-build-bitcode
#$(BITCODE_LD) \
#    $(call host-path, $(PRIVATE_OBJECTS)) \
#    $(PRIVATE_LDFLAGS) \
#    $(PRIVATE_LDLIBS) \
#    -o $(call host-path, $@)
#endef
