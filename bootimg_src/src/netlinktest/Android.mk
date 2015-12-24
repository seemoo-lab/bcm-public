
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	netlinktest.c

LOCAL_MODULE := netlinktest
LOCAL_CFLAGS := -DTARGETENV_android -Dlinux -DLINUX
ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -mabi=aapcs-linux
endif

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := debug

include $(BUILD_EXECUTABLE)
