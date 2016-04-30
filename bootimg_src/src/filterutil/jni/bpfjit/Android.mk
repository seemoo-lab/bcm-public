LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/bpfjit.c

#LOCAL_CFLAGS:=-O2 -g -Ijni/libpcap -Ijni/bpfjit/sljit/sljit_src -DSLJIT_CONFIG_AUTO=1
LOCAL_CFLAGS:=-O2 -g -Ijni/libpcap -Ijni/bpfjit/sljit/sljit_src -DSLJIT_CONFIG_ARM_THUMB2=1

LOCAL_MODULE:= bpfjit
LOCAL_STATIC_LIBRARIES := sljit
include $(BUILD_STATIC_LIBRARY)
include jni/bpfjit/sljit/sljit_src/Android.mk
