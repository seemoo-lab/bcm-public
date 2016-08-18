CC=../../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FILE1=wlc_bmac_recv
FILE2=filter
ENTRY_FUNC1=wlc_bmac_recv
ENTRY_FUNC2=nexmon_filter

all: bcmdhd/bcmdhd.ko $(FILE1).o $(FILE1).elf $(FILE1).bin fw_bcmdhd.bin
nexmon_filter: $(FILE1)_filter.o $(FILE1).elf $(FILE1).bin fw_bcmdhd.bin

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../../kernel/ M=$$PWD/bcmdhd -j2

$(FILE1).o: $(FILE1).c ../../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $(FILE1).c -I../../include -o $(FILE1).o
$(FILE1)_filter.o: $(FILE1).c ../../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -DNEXMON_FILTER -ffreestanding -mthumb -ffunction-sections -fdata-sections -I../../include -c $(FILE1).c -o $(FILE1).o
	

../../wrapper/wrapper.o: FORCE
	make -C ../../wrapper

$(FILE1).elf: $(FILE1).o ../../wrapper/wrapper.o $(FILE1).ld
	$(CC)ld -T $(FILE1).ld -o $(FILE1).elf --gc-sections --print-gc-sections -M

$(FILE1).bin: $(FILE1).elf
	$(CC)objcopy -O binary -j .text.wlc_bmac_recv $(FILE1).elf $(FILE1).bin
$(FILE2).bin: $(FILE1).elf
	$(CC)objcopy -O binary -j .text.nexmon_filter -j .text.sdio_handler $(FILE1).elf $(FILE2).bin

text.bin: $(FILE1).elf
	$(CC)objcopy -O binary -j .text $(FILE1).elf text.bin

fw_bcmdhd.bin: $(FILE1).bin $(FILE2).bin text.bin
	./patcher.py

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

clean: 
	rm -f $(FILE1).bin $(FILE2).bin text.bin $(FILE1).elf $(FILE2).elf $(FILE1).o fw_bcmdhd.bin
	make -C ../../../kernel/ M=$$PWD/bcmdhd clean

FORCE:
