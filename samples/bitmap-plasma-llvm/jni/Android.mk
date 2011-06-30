LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -I/usr/local/google/work/m/frameworks/compile/libbcc/include/
LOCAL_LDFLAGS := -L/usr/local/google/work/m/out/target/product/crespo4g/system/lib/

LOCAL_MODULE    := plasma
LOCAL_SRC_FILES := plasmaLLVM.c
LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lbcc

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := plasma_portable
LOCAL_SRC_FILES := libplasma.c

include $(BUILD_BITCODE)
