all: ../../bin/tcpdump

../../bin/tcpdump: libs/armeabi/tcpdump
	cp libs/armeabi/* ../../bin

libs/armeabi/tcpdump: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
