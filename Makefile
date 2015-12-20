CC=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
CROSS_COMPILE=$(shell pwd)/buildtools/arm-eabi-4.7/bin/arm-eabi-
ARCH=arm
SUBARCH=arm

all: tools

.PHONY: cleanall cleanbuildtools kernel


cleanall: cleanbuildtools



kernel: kernel/arch/arm/boot/zImage-dtb

kernel/arch/arm/boot/zImage-dtb:
	cd kernel && make



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
