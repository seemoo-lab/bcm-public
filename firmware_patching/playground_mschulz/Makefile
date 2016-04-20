CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-
FUNCTIONS = dma_txfast_hook nex_ioctl_handler handle_exceptions tr_pref_abort_hook tr_data_abort_hook interrupt_handler path_to_load_ucode_hook before_before_initialize_memory_hook wlc_txfifo_hook interrupt_enable_hook wlc_ucode_download_hook setup_some_stuff_hook bus_binddev_rom_hook sub_1ECAB0_hook function_with_huge_jump_table_hook handle_ioctl_cmd_hook console_size_r0 console_size_r2
FUNCTIONS_BIN = $(FUNCTIONS:=.bin)

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin fw_bcmdhd.complete.elf

bcmdhd/bcmdhd.ko: FORCE
	if [ "$$ARCH" = "arm" ]; then make -C ../../kernel/ M=$$PWD/bcmdhd -j2; else echo "ERR: run 'source setup_env.sh' first"; fi

fw_bcmdhd.bin: patcher.py patch.elf text.bin dngl_sendpkt.bin $(FUNCTIONS_BIN)
	./patcher.py

patch.o: patch.c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -c $< -o $@

../wrapper/wrapper.o: ../include/wrapper.h
	make -C ../wrapper

bdc_ethernet_ipv6_udp_header_array.o: bdc_ethernet_ipv6_udp_header_array.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $< -o $@

dngl_sendpkt_alternative.o: dngl_sendpkt_alternative.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $< -o $@

patch.elf: patch.ld patch.o ../wrapper/wrapper.o dngl_sendpkt_alternative.o bdc_ethernet_ipv6_udp_header_array.o
	$(CC)ld -T $< --entry=dma_txfast_hook -o $@ --gc-sections --print-gc-sections -M

$(FUNCTIONS_BIN): patch.elf
	$(CC)objcopy -O binary -j .text.$(basename $@) $< $@

text.bin: patch.elf
	$(CC)objcopy -O binary -j .text $< $@

dngl_sendpkt.bin : ../../bootimg_src/firmware/fw_bcmdhd.orig.bin Makefile
	dd if=$< of=$@ bs=1 skip=$$((0x2450)) count=$$((0x464))

fw_bcmdhd.complete.bin : fw_bcmdhd.bin ../../bootimg_src/firmware/rom.bin
	dd if=../../bootimg_src/firmware/rom.bin of=$@ bs=1
	dd if=fw_bcmdhd.bin of=$@ bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary $< $@
	$(CC)objcopy --set-section-flags .data=code $@

clean:
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf patch.o dngl_sendpkt_alternative.o bdc_ethernet_ipv6_udp_header_array.o patch.elf text.bin dngl_sendpkt.bin $(FUNCTIONS_BIN)
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE: