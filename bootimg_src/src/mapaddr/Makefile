all: ../../bin/mapaddr

../../bin/mapaddr: libs/armeabi/mapaddr
	cp libs/armeabi/mapaddr ../../bin

libs/armeabi/mapaddr: Android.mk ../../../buildtools/mapaddr/mapaddr.c
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
