LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := filterutil
LOCAL_SRC_FILES := main.c
LOCAL_C_INCLUDES := $(MY_LOCAL_PATH)/../../libpcap
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_STATIC_LIBRARIES := libpcap

include $(BUILD_EXECUTABLE)    # <-- Use this to build an executable.
include $(MY_LOCAL_PATH)/../../libpcap/Android.mk
