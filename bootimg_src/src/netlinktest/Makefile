all: ../../bin/netlinktest

../../bin/netlinktest: libs/armeabi/netlinktest
	cp libs/armeabi/netlinktest ../../bin

libs/armeabi/netlinktest: netlinktest.c Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
