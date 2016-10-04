CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../kernel/ M=$$PWD/bcmdhd -j2

fw_bcmdhd.bin: ../../bootimg_src/firmware/fw_bcmdhd.6.37.32.RC23.34.43_r639704_hammerhead_mob30y.bin
	cp $< $@

fw_bcmdhd.complete.bin: fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf: fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

install-firmware: fw_bcmdhd.bin
	adb shell 'su -c "mount -o rw,remount /system"' && \
	adb push $< /sdcard/ && \
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /vendor/firmware/fw_bcmdhd.bin"'

clean:
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE: