LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libplasma

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := plasmaLLVM.cpp

LOCAL_C_INCLUDES := $(NDK_ROOT)/../frameworks/compile/libbcc/include

# Workaround. libbcc is not part of NDK
LOCAL_LDFLAGS := $(OUT)/system/lib/libbcc.so

LOCAL_LDFLAGS += -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
