LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -D NUM=7788

LOCAL_MODULE    := hello_llvm
LOCAL_SRC_FILES := hello_llvm.c test.c

include $(BUILD_BITCODE)
