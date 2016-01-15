CC=/usr/bin/arm-none-eabi-
FILE=wlc_bmac_recv
ENTRY_FUNC=wlc_bmac_recv

all: $(FILE).o $(FILE).elf $(FILE).bin

$(FILE).o: $(FILE).c *.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE).c -o $(FILE).o

wrapper.o: wrapper.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c wrapper.c -o wrapper.o

$(FILE).elf: $(FILE).o wrapper.o $(FILE).ld
	$(CC)ld -T $(FILE).ld --entry=$(ENTRY_FUNC) -o $(FILE).elf --gc-sections --print-gc-sections -M

$(FILE).bin: $(FILE).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE).elf $(FILE).bin

clean: 
	rm -f $(FILE).bin $(FILE).elf $(FILE).o wrapper.o
