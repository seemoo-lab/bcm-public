all: ../../bin/iwconfig ../../bin/iwlist ../../bin/iwpriv

../../bin/iwconfig: local/armeabi/iwconfig
	cp $< $@

../../bin/iwlist: local/armeabi/iwlist
	cp $< $@

../../bin/iwpriv: local/armeabi/iwpriv
	cp $< $@

local/armeabi/iwconfig: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

local/armeabi/iwlist: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

local/armeabi/iwpriv: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
