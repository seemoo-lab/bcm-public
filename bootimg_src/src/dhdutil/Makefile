all: ../../bin/dhdutil

../../bin/dhdutil: local/armeabi/dhdutil
	cp local/armeabi/dhdutil ../../bin

local/armeabi/dhdutil: *.c *.h Android.mk
	../../../../android/android-ndk-r10e/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
