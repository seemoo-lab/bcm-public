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
FWPATCH=original_firmware
MSGLEVEL=0x48f

all: tools boot.img

setupenv:
	source setup_env.sh

.PHONY: cleanall cleanbuildtools cleanboot kernel bcmdhd reloadnex su


cleanall: cleanbuildtools cleanboot



kernel: kernel/arch/arm/boot/zImage-dtb

kernel/arch/arm/boot/zImage-dtb: kernel/.config check-nexmon-setup-env
	cd kernel && make



bcmdhd: kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko

$(FWPATCH): FORCE
	cd firmware_patching/$(FWPATCH) && make

kernel/drivers/net/wireless/nexmon/nexmon.ko: FORCE check-nexmon-setup-env
	cd kernel && make modules -j2

su: su.img
	adb push su.img /sdcard/
	adb shell "su -c 'mv /sdcard/su.img /data/su.img'"
	adb push SuperSU.apk /sdcard/
	adb shell "su -c 'mv /sdcard/SuperSU.apk /data/SuperSU.apk'"

boot.img: Makefile mkboot kernel/arch/arm/boot/zImage-dtb $(FWPATCH) kernel/drivers/net/wireless/nexmon/nexmon.ko
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
#	   && sed -i '/^ro.debuggable=/s/=.*/=1/' default.prop \
#	   && sed -i '/^ro.secure=/s/=.*/=0/' default.prop \
#	   && sed -i '/^ro.adb.secure=/s/=.*/=0/' default.prop \
#	   && sed -i '/^on property:ro.kernel.qemu=/s/=.*/=0/' init.rc
	mkdir bootimg_tmp/ramdisk/nexmon
	cp firmware_patching/$(FWPATCH)/bcmdhd/bcmdhd.ko bootimg_tmp/ramdisk/nexmon/
	cp kernel/drivers/net/wireless/nexmon/nexmon.ko bootimg_tmp/ramdisk/nexmon/
	mkdir bootimg_tmp/ramdisk/nexmon/firmware
	cp firmware_patching/$(FWPATCH)/fw_bcmdhd.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_bcmdhd.bin
	cp firmware_patching/wlc_bmac_recv/fw_bcmdhd.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_nexmon.bin
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/bcmdhd.cal
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/nexmon.cal
	cp bootimg_src/firmware/rom.bin bootimg_tmp/ramdisk/nexmon/firmware/rom.bin
	cp bootimg_src/firmware/firmware.map bootimg_tmp/ramdisk/nexmon/firmware/firmware.map
	mkdir bootimg_tmp/ramdisk/nexmon/bin
	cp --preserve=links bootimg_src/bin/* bootimg_tmp/ramdisk/nexmon/bin
	sed -i 's#/su/bin#/nexmon/bin:/su/bin#g' bootimg_tmp/ramdisk/init.environ.rc
	$(MKBOOT)mkbootfs bootimg_tmp/ramdisk | gzip > bootimg_tmp/newramdisk.cpio.gz
	$(MKBOOT)mkbootimg --base 0 --pagesize 2048 --kernel_offset 0x00008000 \
	   --ramdisk_offset 0x02900000 --second_offset 0x00f00000 --tags_offset 0x02700000 \
	   --cmdline 'console=ttyHSL0,115200,n8 androidboot.hardware=hammerhead user_debug=31 maxcpus=2 msm_watchdog_v2.enable=1' \
	   --kernel bootimg_tmp/kernel --ramdisk bootimg_tmp/newramdisk.cpio.gz -o boot.img

cleanboot:
	rm -f boot.img


boot: boot.img
	adb reboot bootloader
	fastboot boot boot.img

reloadfirmware: $(FWPATCH)
	adb push firmware_patching/$(FWPATCH)/fw_bcmdhd.bin /sdcard/
	adb push firmware_patching/$(FWPATCH)/bcmdhd/bcmdhd.ko /sdcard/
	adb shell "su -c 'ifconfig wlan0 down; rmmod bcmdhd; rmmod nexmon; insmod /sdcard/bcmdhd.ko firmware_path=/sdcard/fw_bcmdhd.bin dhd_msg_level=$(MSGLEVEL)'"

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