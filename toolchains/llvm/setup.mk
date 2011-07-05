GDK_TOOLCHAIN_PREFIX := $(GDK_TOOLCHAIN_ROOT)/llvm/prebuilt/

# TODO(Nowar): In future, we use llvm-ndk-cc.
BITCODE_CC       := $(TOOLCHAIN_PREFIX)clang
BITCODE_CFLAGS   := -ccc-host-triple armv7-none-linux-gnueabi -emit-llvm

BITCODE_CPP      := $(TOOLCHAIN_PREFIX)clang++
BITCODE_CPPFLAGS := $(BITCODE_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus

BITCODE_LD       := $(TOOLCHAIN_PREFIX)llvm-link
BITCODE_LDFLAGS  :=

BITCODE_C_INCLUDES := \
  $(GDK_PLATFORM_ROOT)/usr/include

#define cmd-build-bitcode
#$(BITCODE_LD) \
#    $(call host-path, $(PRIVATE_OBJECTS)) \
#    $(PRIVATE_LDFLAGS) \
#    $(PRIVATE_LDLIBS) \
#    -o $(call host-path, $@)
#endef
