NEXMON_CHIP=CHIP_VER_BCM4339
NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_43_r639704
#NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_40_r581243

CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

ifeq ($(NEXMON_FW_VERSION), FW_VER_6_37_32_RC23_34_43_r639704)
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.6.37.32.RC23.34.43_r639704_hammerhead_mob30y.bin
else
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.orig.bin
endif

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin fw_bcmdhd.complete.elf

include ucode_compression_code.mk

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../kernel/ M=$$PWD/bcmdhd -j2

patch.o: patch.c ../include/*.h ucode-compressed.c
	$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=patch.o \
		-fplugin-arg-nexmon-ldfile=patch.generated.ld \
		-fplugin-arg-nexmon-makefile=patch.generated.mk \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

wrapper.o: ../include/wrapper.c
	make -C ../wrapper NEXMON_CHIP=$(NEXMON_CHIP) NEXMON_FW_VERSION=$(NEXMON_FW_VERSION) WRAPPER_LD_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/wrapper.ld WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/wrapper.o

patch.elf: patch.ld patch.o wrapper.o ucode_compression_code.ld
	$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M

fw_bcmdhd.bin: patch.elf $(ORIG_FW)
	cp $(ORIG_FW) $@
	$(CC)objcopy -O binary -j .text $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc seek=$$((0x180020 - 0x180000))
	make -f patch.generated.mk

fw_bcmdhd.complete.bin : fw_bcmdhd.bin ../../bootimg_src/firmware/rom.bin
	dd if=../../bootimg_src/firmware/rom.bin of=$@ bs=1
	dd if=fw_bcmdhd.bin of=$@ bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary $< $@
	$(CC)objcopy --set-section-flags .data=code $@

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

install-firmware: fw_bcmdhd.bin
	adb shell 'su -c "mount -o rw,remount /system"' && \
	adb push $< /sdcard/ && \
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /vendor/firmware/fw_bcmdhd.bin"'

clean-firmware: FORCE
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf patch.o patch.elf ucode-compressed.c ucode-compressed.bin *.generated.* wrapper.o wrapper.ld

clean: clean-firmware
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE:
