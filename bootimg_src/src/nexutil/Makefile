all: ../../bin/nexutil

../../bin/nexutil: libs/armeabi/nexutil
	cp libs/armeabi/nexutil ../../bin

libs/armeabi/nexutil: Android.mk nexutil.c
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
