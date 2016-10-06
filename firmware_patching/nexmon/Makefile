SRCS=$(wildcard *.c) ucode_compressed.c
OBJS=$(SRCS:.c=.o)
Q=@

NEXMON_CHIP=CHIP_VER_BCM4339
NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_43_r639704
#NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_40_r581243

CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

ifeq ($(NEXMON_FW_VERSION), FW_VER_6_37_32_RC23_34_43_r639704)
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.6.37.32.RC23.34.43_r639704_hammerhead_mob30y.bin
else
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.orig.bin
endif

firmware: fw_bcmdhd.bin

driver: bcmdhd/bcmdhd.ko

all: firmware driver

include ucode_compression_code.mk

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../kernel/ M=$$PWD/bcmdhd -j2

%.o: %.c ../include/*.h
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$@ \
		-fplugin-arg-nexmon-ldfile=$(basename $@).generated.ld \
		-fplugin-arg-nexmon-makefile=$(basename $@).generated.mk \
		-fplugin-arg-nexmon-chipver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_CHIP) | $(CC)gcc -E -x c - | tail -n 1` \
		-fplugin-arg-nexmon-fwver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_FW_VERSION) | $(CC)gcc -E -x c - | tail -n 1` \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

%.out: %.c ../include/*.h
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$@ \
		-fplugin-arg-nexmon-ldfile=$(basename $@).generated.ld \
		-fplugin-arg-nexmon-makefile=$(basename $@).generated.mk \
		-fplugin-arg-nexmon-chipver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_CHIP) | $(CC)gcc -E -x c - | tail -n 1` \
		-fplugin-arg-nexmon-fwver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_FW_VERSION) | $(CC)gcc -E -x c - | tail -n 1` \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-E -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

wrapper.o: ../include/wrapper.c
	make -C ../wrapper \
		NEXMON_CHIP=$(NEXMON_CHIP) \
		NEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		WRAPPER_LD_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/$(basename $@).generated.ld \
		WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/$@

patch.elf: patch.ld patch.o wrapper.o $(OBJS)
	$(Q)$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M

fw_bcmdhd.bin: patch.elf $(ORIG_FW)
	cp $(ORIG_FW) $@
	$(CC)objcopy -O binary -j .text.patch $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc status=none seek=$$((0x180020 - 0x180000))
	$(CC)objcopy -O binary -j .text.ucode $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc status=none seek=$$((0x1FD82C - 0x180000)) # 0x1FD820 on .40 firmware
	$(Q)make -f patch.generated.mk Q=$(Q)
	$(Q)make -f radiotap.generated.mk Q=$(Q)
	$(Q)make -f flashpatch.generated.mk Q=$(Q)

fw_bcmdhd.complete.bin : fw_bcmdhd.bin ../../bootimg_src/firmware/rom.bin
	dd if=../../bootimg_src/firmware/rom.bin status=none of=$@ bs=1
	dd if=fw_bcmdhd.bin of=$@ bs=1 status=none seek=1572864

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
	adb shell 'su -c "ifconfig wlan0 down"'

backup-firmware: FORCE
	adb shell 'su -c "cp /vendor/firmware/fw_bcmdhd.bin /sdcard/fw_bcmdhd.orig.bin"'
	adb pull /sdcard/fw_bcmdhd.orig.bin

install-backup: fw_bcmdhd.orig.bin
	adb shell 'su -c "mount -o rw,remount /system"' && \
	adb push $< /sdcard/ && \
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /vendor/firmware/fw_bcmdhd.bin"'
	adb shell 'su -c "ifconfig wlan0 down && ifconfig wlan0 up"'

clean-firmware: FORCE
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf *.o patch.elf ucode_compressed.c ucode_compressed.bin *.generated.* wrapper.ld

clean: clean-firmware
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE:
