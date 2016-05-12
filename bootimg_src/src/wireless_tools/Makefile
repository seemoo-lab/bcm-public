all: ../../bin/iwconfig ../../bin/iwlist ../../bin/iwpriv

../../bin/iwconfig: libs/armeabi/iwconfig
	cp $< $@

../../bin/iwlist: libs/armeabi/iwlist
	cp $< $@

../../bin/iwpriv: libs/armeabi/iwpriv
	cp $< $@

libs/armeabi/iwconfig: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

libs/armeabi/iwlist: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

libs/armeabi/iwpriv: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

clean:
	rm -Rf libs
	rm -Rf local
