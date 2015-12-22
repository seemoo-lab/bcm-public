CC=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
CROSS_COMPILE=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
ARCH=arm
SUBARCH=arm
MKBOOT=$(shell pwd)/buildtools/mkboot/

all: tools

.PHONY: cleanall cleanbuildtools cleanboot kernel bcmdhd


cleanall: cleanbuildtools cleanboot



kernel: kernel/arch/arm/boot/zImage-dtb

kernel/arch/arm/boot/zImage-dtb: kernel/.config
	cd kernel && make



bcmdhd: kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko

kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko : kernel/drivers/net/wireless/bcmdhd/*.c kernel/drivers/net/wireless/bcmdhd/*.h
	cd kernel && make modules



boot.img: kernel/arch/arm/boot/Image kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko
	rm -Rf bootimg_tmp
	mkdir bootimg_tmp
	cd bootimg_tmp && \
	   $(MKBOOT)unmkbootimg -i ../bootimg_src/boot.img && \
	   rm kernel && cp ../kernel/arch/arm/boot/zImage-dtb kernel
	mkdir bootimg_tmp/ramdisk && \
	   cd bootimg_tmp/ramdisk && \
	   gzip -dc ../ramdisk.cpio.gz | cpio -i
	mkdir bootimg_tmp/ramdisk/nexmon
	cp kernel/drivers/net/wireless/bcmdhd/bcmdhd.ko bootimg_tmp/ramdisk/nexmon/
	cp bootimg_src/firmware/fw_bcmdhd.bin bootimg_tmp/ramdisk/nexmon/
	cp bootimg_src/firmware/bcmdhd.cal bootimg_tmp/ramdisk/nexmon/
	mkdir bootimg_tmp/ramdisk/nexmon/bin
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
