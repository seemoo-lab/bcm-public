CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin

bcmdhd/bcmdhd.ko: FORCE
	if [ "$$ARCH" = "arm" ]; then make -C ../../kernel/ M=$$PWD/bcmdhd -j2; else echo "ERR: run 'source setup_env.sh' first"; fi

fw_bcmdhd.bin: ../../bootimg_src/firmware/fw_bcmdhd.orig.bin
	cp $< $@

fw_bcmdhd.complete.bin: fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf: fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

clean:
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE: