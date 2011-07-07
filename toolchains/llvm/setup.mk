# FIXME(Nowar): Use llvm-ndk-cc instead of clang.
#TARGET_CC := $(TOOLCHAIN_PREBUILT_ROOT)/llvm-ndk-cc
#TARGET_CFLAGS :=
#
#TARGET_CXX := $(TARGET_CC)
#TARGET_CXXFLAGS := $(TARGET_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus
#
#TARGET_LD := $(TOOLCHAIN_PREBUILT_ROOT)/llvm-ndk-link
#TARGET_LDFLAGS :=

TARGET_C_INCLUDES := $(GDK_PLATFORMS_ROOT)/android-portable/arch-llvm/usr/include

TARGET_CC       := clang
TARGET_CFLAGS   := -ccc-host-triple armv7-none-linux-gnueabi -emit-llvm

TARGET_CXX      := clang++
TARGET_CXXFLAGS := $(TARGET_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus

TARGET_LD       := llvm-link
TARGET_LDFLAGS  :=

define cmd-link-bitcodes
$(TARGET_LD) \
  $(call host-path, $(PRIVATE_OBJECTS)) \
  -o $(call host-path,$@)
endef
