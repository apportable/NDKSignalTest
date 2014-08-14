LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NDKSignalTest
LOCAL_SRC_FILES := NDKSignalTest.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
