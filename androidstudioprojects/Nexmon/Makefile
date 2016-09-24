all: app/src/main/assets/nexmon/libfakeioctl.so \
	app/src/main/assets/nexmon/nexutil \
	app/src/main/assets/nexmon/dhdutil \
	app/src/main/assets/nexmon/tcpdump \
	app/src/main/assets/nexmon/aircrack-ng \
	app/src/main/assets/nexmon/nc \
	app/src/main/assets/nexmon/iw \
	app/src/main/assets/nexmon/iwconfig \
	app/src/main/assets/nexmon/mdk3 \
	app/src/main/assets/nexmon/fw_bcmdhd.bin

app/src/main/assets/nexmon/libfakeioctl.so: ../../bootimg_src/src/libfakeioctl/libs/armeabi/libfakeioctl.so TOOLS
	cp $< $@

app/src/main/assets/nexmon/nexutil: ../../bootimg_src/src/nexutil/libs/armeabi/nexutil TOOLS
	cp $< $@

app/src/main/assets/nexmon/dhdutil: ../../bootimg_src/src/dhdutil/libs/armeabi/dhdutil TOOLS
	cp $< $@

app/src/main/assets/nexmon/tcpdump: ../../bootimg_src/src/tcpdump/libs/armeabi/tcpdump TOOLS
	cp $< $@

app/src/main/assets/nexmon/aircrack-ng: ../../bootimg_src/src/aircrack-ng/libs/armeabi/aircrack-ng TOOLS
	cp ../../bootimg_src/src/aircrack-ng/libs/armeabi/* app/src/main/assets/nexmon

app/src/main/assets/nexmon/nc: ../../bootimg_src/src/netcat/libs/armeabi/nc TOOLS
	cp $< $@

app/src/main/assets/nexmon/iw: ../../bootimg_src/src/iw/libs/armeabi/iw TOOLS
	cp $< $@

app/src/main/assets/nexmon/iwconfig: ../../bootimg_src/src/wireless_tools/libs/armeabi/iwconfig TOOLS
	cp ../../bootimg_src/src/wireless_tools/libs/armeabi/* app/src/main/assets/nexmon

app/src/main/assets/nexmon/mdk3: ../../bootimg_src/src/mdk3/libs/armeabi/mdk3 TOOLS
	cp $< $@

app/src/main/assets/nexmon/fw_bcmdhd.bin: ../../firmware_patching/nexmon/fw_bcmdhd.bin NEXMON
	cp $< $@

clean: FORCE
	rm -f app/src/main/assets/nexmon/*

NEXMON: FORCE
	cd ../../firmware_patching/nexmon/ && make

TOOLS: FORCE
	cd ../../bootimg_src/src && make

FORCE: