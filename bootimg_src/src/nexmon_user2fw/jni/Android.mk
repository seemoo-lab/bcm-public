LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := user2fw 
LOCAL_SRC_FILES := main.c
LOCAL_CFLAGS := -Wall # whatever g++ flags you like
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog   # whatever ld flags you like

include $(BUILD_EXECUTABLE)    # <-- Use this to build an executable.
