SHELL=/bin/bash

all : dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf

dngl_sendpkt.bin : ../../bootimg_src/firmware/fw_bcmdhd.orig.bin Makefile
	dd if=../../bootimg_src/firmware/fw_bcmdhd.orig.bin of=dngl_sendpkt.bin bs=1 skip=$$((0x2450)) count=$$((0x464))

fw_bcmdhd.bin : binary_patcher.py dngl_sendpkt.bin
	./binary_patcher.py

fw_bcmdhd.complete.bin : fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

clean:
	rm -f dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
