all: ../../bin/netlinktest

../../bin/netlinktest: local/armeabi/netlinktest
	cp local/armeabi/netlinktest ../../bin

local/armeabi/netlinktest: netlinktest.c Android.mk
	../../../../android/android-ndk-r10e/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
