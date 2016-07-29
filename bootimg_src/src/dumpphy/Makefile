all: ../../bin/dumpphy

../../bin/dumpphy: libs/armeabi/dumpphy
	cp libs/armeabi/dumpphy ../../bin

libs/armeabi/dumpphy: Android.mk dumpphy.c
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
