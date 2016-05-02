all: ../../bin/iw

../../bin/iw: local/armeabi/iw
	cp local/armeabi/iw ../../bin

local/armeabi/iw: Android.mk
#ifneq ("$(wildcard $(../../../../android/android-ndk-r10e/ndk-build))","")
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21
#else
#	ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21
#endif

clean:
	rm -Rf libs
	rm -Rf local
