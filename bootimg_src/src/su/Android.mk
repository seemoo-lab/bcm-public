LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := su
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_CFLAGS += -Werror
LOCAL_STATIC_LIBRARIES := libsqlite libcutils libc
LOCAL_SRC_FILES := su.c activity.c utils.c daemon.c pts.c
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_MODULE := libsqlite
LOCAL_SRC_FILES := $(LOCAL_PATH)/../shared_system_libs/libsqlite.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../sqlite/dist
include $(PREBUILT_SHARED_LIBRARY)
