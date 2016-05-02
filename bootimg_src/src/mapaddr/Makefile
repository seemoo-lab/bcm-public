all: ../../bin/mapaddr

../../bin/mapaddr: local/armeabi/mapaddr
	cp local/armeabi/mapaddr ../../bin

local/armeabi/mapaddr: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
