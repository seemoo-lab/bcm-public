CC=../../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE=user2fw
ENTRY_FUNC=sdio_handler

all: bcmdhd/bcmdhd.ko $(FILE).o $(FILE).elf $(FILE).bin fw_bcmdhd.bin

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../../kernel/ M=$$PWD/bcmdhd -j2

$(FILE).o: $(FILE).c ../../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -I../../include -c $(FILE).c -o $(FILE).o

../../wrapper/wrapper.o: FORCE
	make -C ../../wrapper

$(FILE).elf: $(FILE).o ../../wrapper/wrapper.o $(FILE).ld
	$(CC)ld -T $(FILE).ld --entry=$(ENTRY_FUNC) -o $(FILE).elf --gc-sections

$(FILE).bin: $(FILE).elf
	$(CC)objcopy -O binary -j .text -j .data -j .bss -j .rodata $(FILE).elf $(FILE).bin

fw_bcmdhd.bin: $(FILE).bin
	./patcher.py

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

clean: 
	rm -f $(FILE).bin $(FILE).elf $(FILE).o fw_bcmdhd.bin
	make -C ../../../kernel/ M=$$PWD/bcmdhd clean

FORCE:
