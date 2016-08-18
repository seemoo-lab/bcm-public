CC=../../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FUNCTIONS = nex_ioctl_handler
FUNCTIONS_BIN = $(FUNCTIONS:=.bin)

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin fw_bcmdhd.complete.elf

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../../kernel/ M=$$PWD/bcmdhd -j2

fw_bcmdhd.bin: patcher.py patch.elf text.bin $(FUNCTIONS_BIN)
	./patcher.py

patch.o: patch.c ../../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../../include -c $< -o $@

../../wrapper/wrapper.o: FORCE
	make -C ../../wrapper

patch.elf: patch.ld patch.o ../../wrapper/wrapper.o
	$(CC)ld -T $< --entry=dma_txfast_hook -o $@ --gc-sections --print-gc-sections -M

$(FUNCTIONS_BIN): patch.elf
	$(CC)objcopy -O binary -j .text.$(basename $@) $< $@

text.bin: patch.elf
	$(CC)objcopy -O binary -j .text $< $@

fw_bcmdhd.complete.bin : fw_bcmdhd.bin ../../../bootimg_src/firmware/rom.bin
	dd if=../../../bootimg_src/firmware/rom.bin of=$@ bs=1
	dd if=fw_bcmdhd.bin of=$@ bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary $< $@
	$(CC)objcopy --set-section-flags .data=code $@

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

clean:
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf patch.o patch.elf text.bin $(FUNCTIONS_BIN)
	make -C ../../../kernel/ M=$$PWD/bcmdhd clean

FORCE: