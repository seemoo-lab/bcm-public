CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE=wlc_bmac_recv
ENTRY_FUNC=wlc_bmac_recv

all: $(FILE).o $(FILE).elf $(FILE).bin fw_bcmdhd.bin

$(FILE).o: $(FILE).c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE).c -o $(FILE).o

../wrapper/wrapper.o: ../wrapper/wrapper.c
	make -C ../wrapper

$(FILE).elf: $(FILE).o ../wrapper/wrapper.o $(FILE).ld
	$(CC)ld -T $(FILE).ld --entry=$(ENTRY_FUNC) -o $(FILE).elf --gc-sections --print-gc-sections -M

$(FILE).bin: $(FILE).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE).elf $(FILE).bin

fw_bcmdhd.bin: $(FILE).bin
	./patcher.py

clean: 
	rm -f $(FILE).bin $(FILE).elf $(FILE).o fw_bcmdhd.bin
