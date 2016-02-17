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

all: tools boot.img

.PHONY: cleanall cleanbuildtools cleanboot kernel bcmdhd reloadnex su


cleanall: cleanbuildtools cleanboot



kernel: kernel/arch/arm/boot/zImage-dtb

kernel/arch/arm/boot/zImage-dtb: kernel/.config
	cd kernel && make



bcmdhd: kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko

kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko : kernel/drivers/net/wireless/bcmdhd/*.h kernel/drivers/net/wireless/bcmdhd/*.c kernel/drivers/net/wireless/bcmdhd/Makefile
	cd kernel && make

kernel/drivers/net/wireless/nexmon/nexmon.ko : kernel/drivers/net/wireless/nexmon/*.h kernel/drivers/net/wireless/nexmon/*.c kernel/drivers/net/wireless/nexmon/Makefile
	cd kernel && make

kernel/drivers/net/wireless/nexdhd/nexdhd.ko : kernel/drivers/net/wireless/nexdhd/*.h kernel/drivers/net/wireless/nexdhd/*.c kernel/drivers/net/wireless/nexdhd/Makefile
	cd kernel && make

su: su.img
	adb push su.img /sdcard/
	adb shell "su -c 'mv /sdcard/su.img /data/su.img'"
	adb push SuperSU.apk /sdcard/
	adb shell "su -c 'mv /sdcard/SuperSU.apk /data/SuperSU.apk'"

boot.img: Makefile kernel/arch/arm/boot/zImage-dtb kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko kernel/drivers/net/wireless/nexmon/nexmon.ko  kernel/drivers/net/wireless/nexdhd/nexdhd.ko 
	rm -Rf bootimg_tmp
	mkdir bootimg_tmp
	cd bootimg_tmp && \
	   $(MKBOOT)unmkbootimg -i ../bootimg_src/boot.img && \
	   rm kernel && cp ../kernel/arch/arm/boot/zImage-dtb kernel
	mkdir bootimg_tmp/ramdisk && \
	   cd bootimg_tmp/ramdisk && \
	   gzip -dc ../ramdisk.cpio.gz | cpio -i && \
	   sed -i '/service wpa_supplicant/,+11 s/^/#/' init.hammerhead.rc && \
	   sed -i '/service p2p_supplicant/,+14 s/^/#/' init.hammerhead.rc
	mkdir bootimg_tmp/ramdisk/nexmon
	cp kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko bootimg_tmp/ramdisk/nexmon/
	cp kernel/drivers/net/wireless/nexmon/nexmon.ko bootimg_tmp/ramdisk/nexmon/
	cp kernel/drivers/net/wireless/nexdhd/nexdhd.ko bootimg_tmp/ramdisk/nexmon/
	mkdir bootimg_tmp/ramdisk/nexmon/firmware
	cp bootimg_src/firmware/fw_bcmdhd.orig.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_bcmdhd.bin
#	cp firmware_patching/dma_txfast_path/fw_bcmdhd.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_bcmdhd.bin
	cp bootimg_src/firmware/fw_nexmon.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_nexmon.bin
	cp bootimg_src/firmware/fw_nexdhd.bin bootimg_tmp/ramdisk/nexmon/firmware/fw_nexdhd.bin
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/bcmdhd.cal
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/nexmon.cal
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/firmware/nexdhd.cal
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

reloadnex: kernel/drivers/net/wireless/nexdhd/nexdhd.ko
	adb push kernel/drivers/net/wireless/nexdhd/nexdhd.ko /sdcard/
	adb shell "su -c 'rmmod nexdhd; rmmod bcmdhd; rmmod nexmon; insmod /sdcard/nexdhd.ko; lsmod'"

reloadnexfirmware: 
	adb push nexdhd_firmware/firmware.bin /sdcard/
	adb shell "su -c 'ifconfig wlan0 down; ifconfig wlan0 up; /nexmon/bin/dhdutil -i wlan0 download /sdcard/firmware.bin'"

reloadbcmdhdfirmware:
	adb push firmware_patching/dma_txfast_path/fw_bcmdhd.bin /sdcard/
	adb push kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko /sdcard/
	adb shell "su -c 'rmmod nexdhd; rmmod bcmdhd; rmmod nexmon; insmod /sdcard/bcmdhd.ko firmware_path=/sdcard/fw_bcmdhd.bin dhd_msg_level=0x49f'"

tools: buildtools/mkboot/mkbootimg buildtools/mkboot/unmkbootimg buildtools/mkboot/mkbootfs

buildtools/mkboot/mkbootimg: src/mkbootimg/mkbootimg.c
	mkdir -p buildtools/mkboot
	cd src/ && \
	   gcc -std=c99 -o ../buildtools/mkboot/mkbootimg libmincrypt/*.c mkbootimg/mkbootimg.c -Iinclude

buildtools/mkboot/unmkbootimg: src/mkbootimg/unmkbootimg.c
	mkdir -p buildtools/mkboot
	cd src/ && \
	   gcc -std=c99 -o ../buildtools/mkboot/unmkbootimg libmincrypt/*.c mkbootimg/unmkbootimg.c -Iinclude

buildtools/mkboot/mkbootfs: src/cpio/mkbootfs.c
	mkdir -p buildtools/mkboot
	cd src/ && \
	   gcc -o ../buildtools/mkboot/mkbootfs cpio/mkbootfs.c libcutils/fs_config.c -Iinclude

cleanbuildtools:
	rm -f buildtools/mkboot/*
