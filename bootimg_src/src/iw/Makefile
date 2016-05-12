all: ../../bin/iw

../../bin/iw: libs/armeabi/iw
	cp libs/armeabi/iw ../../bin

libs/armeabi/iw: Android.mk
#ifneq ("$(wildcard $(../../../../android/android-ndk-r10e/ndk-build))","")
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21
#else
#	ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21
#endif

clean:
	rm -Rf libs
	rm -Rf local
