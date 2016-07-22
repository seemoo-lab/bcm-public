all: jni/Android.mk
	$(NDK_ROOT)/ndk-build TARGET_PLATFORM=android-21

clean:
	$(NDK_ROOT)/ndk-build clean

push: libs/armeabi/inject
	adb push libs/armeabi/inject /sdcard/
	adb shell "su -c 'cp /sdcard/inject /data/'"
	adb shell "su -c 'chmod 777 /data/inject'"
