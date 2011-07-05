LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hellollvmDSO
LOCAL_CFLAGS    := -D NUM=7788
LOCAL_SRC_FILES := hello_llvm.c test.cpp

include $(BUILD_SHARED_LIBRARY)
