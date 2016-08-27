#/**************************************************************************
#*                                                                         *
#*          ###########   ###########   ##########    ##########           *
#*         ############  ############  ############  ############          *
#*         ##            ##            ##   ##   ##  ##        ##          *
#*         ##            ##            ##   ##   ##  ##        ##          *
#*         ###########   ####  ######  ##   ##   ##  ##    ######          *
#*          ###########  ####  #       ##   ##   ##  ##    #    #          *
#*                   ##  ##    ######  ##   ##   ##  ##    #    #          *
#*                   ##  ##    #       ##   ##   ##  ##    #    #          *
#*         ############  ##### ######  ##   ##   ##  ##### ######          *
#*         ###########    ###########  ##   ##   ##   ##########           *
#*                                                                         *
#*            S E C U R E   M O B I L E   N E T W O R K I N G              *
#*                                                                         *
#* Warning:                                                                *
#*                                                                         *
#* Our software may damage your hardware and may void your hardware’s      *
#* warranty! You use our tools at your own risk and responsibility!        *
#*                                                                         *
#* License:                                                                *
#* Copyright (c) 2015 NexMon Team                                          *
#*                                                                         *
#* Permission is hereby granted, free of charge, to any person obtaining   *
#* a copy of this software and associated documentation files (the         *
#* "Software"), to deal in the Software without restriction, including     *
#* without limitation the rights to use, copy, modify, merge, publish,     *
#* distribute copies of the Software, and to permit persons to whom the    *
#* Software is furnished to do so, subject to the following conditions:    *
#*                                                                         *
#* The above copyright notice and this permission notice shall be included *
#* in all copies or substantial portions of the Software.                  *
#*                                                                         *
#* Any use of the Software which results in an academic publication or     *
#* other publication which includes a bibliography must include a citation *
#* to the author's publication "M. Schulz, D. Wegemer and M. Hollick.      *
#* NexMon: A Cookbook for Firmware Modifications on Smartphones to Enable  *
#* Monitor Mode.".                                                         *
#*                                                                         *
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS *
#* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              *
#* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
#* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
#* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,    *
#* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE       *
#* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                  *
#*                                                                         *
#**************************************************************************/

CC=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
CROSS_COMPILE=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
ARCH=arm
SUBARCH=arm
MKBOOT=$(shell pwd)/buildtools/mkboot/
FWPATCH=nexmon
MSGLEVEL=0x48f

all: boot.img

setupenv:
	source setup_env.sh

.PHONY: cleanall cleanbuildtools cleanboot kernel bcmdhd su


cleanall: cleanbuildtools cleanboot



kernel: kernel/arch/arm/boot/zImage-dtb

kernel/arch/arm/boot/zImage-dtb: kernel/.config check-nexmon-setup-env
	cd kernel && make

bcmdhd: kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko

$(FWPATCH): FORCE
	cd firmware_patching/$(FWPATCH) && make

su: su.img
	adb push su.img /sdcard/
	adb shell "su -c 'mv /sdcard/su.img /data/su.img'"
	adb push SuperSU.apk /sdcard/
	adb shell "su -c 'mv /sdcard/SuperSU.apk /data/SuperSU.apk'"

boot.img: Makefile mkboot bootimg_src kernel/arch/arm/boot/zImage-dtb $(FWPATCH)
	rm -Rf bootimg_tmp
	mkdir bootimg_tmp
	cd bootimg_tmp && \
	   $(MKBOOT)unmkbootimg -i ../bootimg_src/boot.img && \
	   rm kernel && cp ../kernel/arch/arm/boot/zImage-dtb kernel
	mkdir bootimg_tmp/ramdisk && \
	   cd bootimg_tmp/ramdisk && \
	   gzip -dc ../ramdisk.cpio.gz | cpio -i \
	   && sed -i '/service wpa_supplicant/,+11 s/^/#/' init.hammerhead.rc \
	   && sed -i '/service p2p_supplicant/,+14 s/^/#/' init.hammerhead.rc \
	   && printf "none /nexmon/tmp/bin tmpfs size=1M defaults\n" >> fstab.hammerhead \
	   && printf "none /system/bin aufs br:/nexmon/tmp/bin:/nexmon/bin:/system/bin defaults\n" >> fstab.hammerhead \
	   && printf "\n\nservice su_daemon /nexmon/bin/su --daemon\n    oneshot\n    class late_start\n    user root\n" >> init.rc
	mkdir bootimg_tmp/ramdisk/nexmon
	cp firmware_patching/$(FWPATCH)/bcmdhd/bcmdhd.ko bootimg_tmp/ramdisk/nexmon/
	mkdir bootimg_tmp/ramdisk/nexmon/firmware
	cp firmware_patching/$(FWPATCH)/fw_bcmdhd.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_bcmdhd.bin
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/bcmdhd.cal
	cp bootimg_src/firmware/rom.bin bootimg_tmp/ramdisk/nexmon/firmware/rom.bin
	cp bootimg_src/firmware/firmware.map bootimg_tmp/ramdisk/nexmon/firmware/firmware.map
	mkdir bootimg_tmp/ramdisk/nexmon/bin
	cp --preserve=links bootimg_src/bin/* bootimg_tmp/ramdisk/nexmon/bin
	# create directories where we mount temporary file systems (tmpfs) used as writable
	# branches for the aufs overlay file system 
	mkdir bootimg_tmp/ramdisk/nexmon/tmp
	mkdir bootimg_tmp/ramdisk/nexmon/tmp/bin
	# copy an init variant with permissive selinux settings to avoid problems with 
	# the aufs overlay filesystem that does not set the correct selinux contexts its fiiles
	rm bootimg_tmp/ramdisk/init
	cp bootimg_src/root/init-selinux-permissive bootimg_tmp/ramdisk/init
	$(MKBOOT)mkbootfs bootimg_tmp/ramdisk | gzip > bootimg_tmp/newramdisk.cpio.gz
	$(MKBOOT)mkbootimg --base 0 --pagesize 2048 --kernel_offset 0x00008000 \
	   --ramdisk_offset 0x02900000 --second_offset 0x00f00000 --tags_offset 0x02700000 \
	   --cmdline 'console=ttyHSL0,115200,n8 androidboot.hardware=hammerhead user_debug=31 maxcpus=2 msm_watchdog_v2.enable=1' \
	   --kernel bootimg_tmp/kernel --ramdisk bootimg_tmp/newramdisk.cpio.gz -o boot.img

bootimg_src: FORCE
	make -C bootimg_src

cleanboot:
	rm -f boot.img

dumpboot: FORCE
	rm -rf bootimg_src/boot.img
	adb shell "su -c 'dd if=/dev/block/mmcblk0p19 of=/sdcard/boot.img'"
	adb pull /sdcard/boot.img bootimg_src/boot.img

boot: boot.img
	adb reboot bootloader
	fastboot boot boot.img

flash: boot.img
	adb reboot bootloader
	fastboot flash boot boot.img
	fastboot reboot

reboot: FORCE
	adb reboot bootloader
	fastboot boot boot.img	

reloadfirmware: $(FWPATCH)
	adb push firmware_patching/$(FWPATCH)/fw_bcmdhd.bin /sdcard/
	adb push firmware_patching/$(FWPATCH)/bcmdhd/bcmdhd.ko /sdcard/
	adb shell "su -c 'ifconfig wlan0 down; rmmod bcmdhd; insmod /sdcard/bcmdhd.ko firmware_path=/sdcard/fw_bcmdhd.bin dhd_msg_level=$(MSGLEVEL)'"

setupnetwork:
	adb shell "su -c 'ifconfig wlan0 down && ifconfig wlan0 up && wpa_supplicant -Dnl80211 -iwlan0 -c/data/misc/wifi/wpa_supplicant.conf -I/system/etc/wifi/wpa_supplicant_overlay.conf &'"
	adb shell "su -c 'sleep 1 && iwconfig wlan0 essid seemoo-jamming && sleep 2 && ifconfig wlan0 192.168.200.101'"

mkboot:
	cd buildtools/mkboot && make

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

FORCE: