all: ../../bin/mdk3

../../bin/mdk3: local/armeabi/mdk3
	cp local/armeabi/mdk3 ../../bin

local/armeabi/mdk3: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
