all: jni/Android.mk
	ndk-build TARGET_PLATFORM=android-21

clean:
	ndk-build clean

push: libs/armeabi/filterutil
	adb push libs/armeabi/filterutil /sdcard/
	adb shell "su -c 'cp /sdcard/filterutil /data/'"
	adb shell "su -c 'chmod 777 /data/filterutil'"
