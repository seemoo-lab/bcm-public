all: ../../bin/su

../../bin/su: libs/armeabi/su
	cp libs/armeabi/* ../../bin

libs/armeabi/su: Android.mk SYSTEM_LIBS
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-23

SYSTEM_LIBS: FORCE
	cd ../shared_system_libs && make

clean:
	rm -Rf libs
	rm -Rf local

FORCE:
