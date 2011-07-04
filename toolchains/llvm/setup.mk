GDK_TOOLCHAIN_PREFIX := $(GDK_TOOLCHAIN_ROOT)/toolchains/llvm/prebuilt/

# FIXME(Nowar): In future, we will use llvm-ndk-cc
BITCODE_CC       := $(TOOLCHAIN_PREFIX)clang
BITCODE_CFLAGS   := -emit-llvm

BITCODE_CXX      := $(TOOLCHAIN_PREFIX)clang++
BITCODE_CXXFLAGS := $(BITCODE_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus

BITCODE_LD       := $(TOOLCHAIN_PREFIX)llvm-link
BITCODE_LDFLAGS  :=

BITCODE_C_INCLUDES := \
  $(GDK_PLATFORM_ROOT)/usr/include

define ev-compile-c-to-bc
_SRC := $$(LOCAL_PATH)/$(1)
_OBJ := $(GDK_PROJECT_PATH)/$(2)
_FLAGS := $$(LOCAL_CFLAGS) \
          $$(BITCODE_CFLAGS) \
          -I $$(LOCAL_C_INCLUDES) \
          -I $$(BITCODE_C_INCLUDES) \
          -c


compile-c-to-bc = $(eval $(call ev-compile-c-to-bc,$1,$(1:%.c=%.bc)))

define cmd-build-bitcode
$(BITCODE_LD) \
    $(call host-path, $(PRIVATE_OBJECTS)) \
    $(PRIVATE_LDFLAGS) \
    $(PRIVATE_LDLIBS) \
    -o $(call host-path, $@)
endef
