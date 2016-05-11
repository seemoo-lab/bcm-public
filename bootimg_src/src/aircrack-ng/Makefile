PROGS = airbase-ng airdecap-ng aireplay-ng airolib-ng airtun-ng buddy-ng ivstools tkiptun-ng wpaclean aircrack-ng airdecloak-ng airodump-ng airserv-ng besside-ng easside-ng kstats makeivs-ng packetforge-ng wesside-ng

all: local/armeabi/aircrack-ng
	cd local/armeabi/ && cp $(PROGS) ../../../../bin

local/armeabi/aircrack-ng: Android.mk libssl.so libcrypto.so libsqlite.so
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

libssl.so: 
	adb pull /system/lib/libssl.so .

libcrypto.so: 
	adb pull /system/lib/libcrypto.so .

libsqlite.so: 
	adb pull /system/lib/libsqlite.so .

clean:
	rm -Rf libs
	rm -Rf local
