TOOLCHAIN_PREFIX := $(TOOLCHAIN_PREFIX:%-=%)

TARGET_CC       := $(TOOLCHAIN_PREFIX)llvm-ndk-cc
TARGET_CFLAGS   :=

TARGET_CXX      := $(TOOLCHAIN_PREFIX)llvm-ndk-cc
TARGET_CXXFLAGS := $(TARGET_CFLAGS) -fno-exceptions -fno-rtti -D __cplusplus

TARGET_LD       := $(TOOLCHAIN_PREFIX)llvm-ndk-link
TARGET_LDFLAGS  :=

TARGET_AR       := ar
TARGET_ARFLAGS  := crs

TARGET_C_INCLUDES := \
    $(SYSROOT)/usr/include

#
# Break build/core/default-build-commands.mk rules
#
TARGET_NO_UNDEFINED_LDFLAGS := 
TARGET_NO_EXECUTE_CFLAGS  := 
TARGET_NO_EXECUTE_LDFLAGS :=
TARGET_LDLIBS := 
cmd-strip := 

define cmd-build-bitcode
$(TARGET_LD) \
		$(call host-path, $(PRIVATE_OBJECTS)) \
		$(PRIVATE_LDFLAGS) \
		$(PRIVATE_LDLIBS) \
		-o $(call host-path, $@)
endef
