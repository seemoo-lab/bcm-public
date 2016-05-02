LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := system-v-search
LOCAL_SRC_FILES = \
	tsearch.c \
	tfind.c \
	tdestroy.c \
	tdelete.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)
