LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libplasma

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := plasmaLLVM.cpp

LOCAL_C_INCLUDES := frameworks/compile/libbcc/include

LOCAL_SHARED_LIBRARIES := libm liblog libjnigraphics libbcc

include $(BUILD_SHARED_LIBRARY)
