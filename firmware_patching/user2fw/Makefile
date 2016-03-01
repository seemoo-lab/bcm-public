CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE=user2fw
ENTRY_FUNC=sdio_handler

all: $(FILE).o $(FILE).elf $(FILE).bin fw_bcmdhd.bin

$(FILE).o: $(FILE).c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE).c -o $(FILE).o

../wrapper/wrapper.o: ../include/wrapper.h
	make -C ../wrapper

$(FILE).elf: $(FILE).o ../wrapper/wrapper.o $(FILE).ld
	$(CC)ld -T $(FILE).ld --entry=$(ENTRY_FUNC) -o $(FILE).elf --gc-sections

$(FILE).bin: $(FILE).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE).elf $(FILE).bin

fw_bcmdhd.bin: $(FILE).bin
	./patcher.py

clean: 
	rm -f $(FILE).bin $(FILE).elf $(FILE).o fw_bcmdhd.bin
