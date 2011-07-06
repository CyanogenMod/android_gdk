LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -I/usr/local/google/work/m/frameworks/compile/libbcc/include/ \
                -I/home/nowar/cyanogen/frameworks/compile/libbcc/include/ \
                -I/home/nowar/cyanogen/ndk/platforms/android-9/arch-arm/usr/include
LOCAL_LDFLAGS := -L/usr/local/google/work/m/out/target/product/crespo4g/system/lib/ \
                 -L/home/nowar/cyanogen/out/target/product/crespo/system/lib/

LOCAL_MODULE    := plasma
LOCAL_SRC_FILES := plasmaLLVM.cpp
LOCAL_LDLIBS    := -lm -llog -ljnigraphics -lbcc

include $(BUILD_SHARED_LIBRARY)
