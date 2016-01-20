CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE=dma_txfast_hook

all : dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf $(FILE).bin

$(FILE).o: $(FILE).c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE).c -o $(FILE).o

../wrapper/wrapper.o: ../wrapper/wrapper.c
	make -C ../wrapper

$(FILE).elf: $(FILE).o ../wrapper/wrapper.o $(FILE).ld
	$(CC)ld -T $(FILE).ld --entry=$(FILE) -o $(FILE).elf --gc-sections --print-gc-sections -M

$(FILE).bin: $(FILE).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE).elf $(FILE).bin


dngl_sendpkt.bin : ../../bootimg_src/firmware/fw_bcmdhd.orig.bin Makefile
	dd if=../../bootimg_src/firmware/fw_bcmdhd.orig.bin of=dngl_sendpkt.bin bs=1 skip=$$((0x2450)) count=$$((0x464))

fw_bcmdhd.bin : patcher.py dngl_sendpkt.bin
	./patcher.py

fw_bcmdhd.complete.bin : fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

clean:
	rm -f dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf $(FILE).o $(FILE).bin $(FILE).elf
