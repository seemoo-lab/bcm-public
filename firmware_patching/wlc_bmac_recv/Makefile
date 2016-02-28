CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE1=wlc_bmac_recv
FILE2=filter
ENTRY_FUNC1=wlc_bmac_recv
ENTRY_FUNC2=nexmon_filter

all: $(FILE1).o $(FILE1).elf $(FILE1).bin fw_bcmdhd.bin

$(FILE1).o: $(FILE1).c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE1).c -o $(FILE1).o
$(FILE2).o: $(FILE2).c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE2).c -o $(FILE2).o

../wrapper/wrapper.o: ../include/wrapper.h
	make -C ../wrapper
../wrapper/wrapper_filter.o: ../include/wrapper_filter.h
	make -C ../wrapper

$(FILE1).elf: $(FILE1).o ../wrapper/wrapper.o $(FILE1).ld
	$(CC)ld -T $(FILE1).ld --entry=$(ENTRY_FUNC1) -o $(FILE1).elf --gc-sections 
$(FILE2).elf: $(FILE2).o ../wrapper/wrapper_filter.o $(FILE2).ld
	$(CC)ld -T $(FILE2).ld --entry=$(ENTRY_FUNC2) -o $(FILE2).elf --gc-sections

$(FILE1).bin: $(FILE1).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE1).elf $(FILE1).bin
$(FILE2).bin: $(FILE2).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE2).elf $(FILE2).bin

fw_bcmdhd.bin: $(FILE1).bin $(FILE2).bin
	./patcher.py

clean: 
	rm -f $(FILE1).bin $(FILE1).elf $(FILE1).o $(FILE2).bin $(FILE2).elf $(FILE2).o fw_bcmdhd.bin
