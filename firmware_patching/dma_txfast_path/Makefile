CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all : dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf wlc_txfifo_hook.bin

patch.o: patch.c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -c $< -o $@

bdc_ethernet_ipv6_udp_header_array.o: bdc_ethernet_ipv6_udp_header_array.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $< -o $@

../wrapper/wrapper.o: ../include/wrapper.h
	make -C ../wrapper

dngl_sendpkt_alternative.o: dngl_sendpkt_alternative.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c $< -o $@

patch.elf: patch.ld patch.o ../wrapper/wrapper.o dngl_sendpkt_alternative.o bdc_ethernet_ipv6_udp_header_array.o
	$(CC)ld -T $< --entry=dma_txfast_hook -o $@ --gc-sections --print-gc-sections -M

dma_txfast_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.dma_txfast_hook $< $@

wlc_txfifo_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.wlc_txfifo_hook $< $@

interrupt_enable_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.interrupt_enable_hook $< $@

setup_some_stuff_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.setup_some_stuff_hook $< $@

bus_binddev_rom_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.bus_binddev_rom_hook $< $@

sub_1ECAB0_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.sub_1ECAB0_hook $< $@

text.bin: patch.elf
	$(CC)objcopy -O binary -j .text $< $@

dngl_sendpkt.bin : ../../bootimg_src/firmware/fw_bcmdhd.orig.bin Makefile
	dd if=../../bootimg_src/firmware/fw_bcmdhd.orig.bin of=dngl_sendpkt.bin bs=1 skip=$$((0x2450)) count=$$((0x464))

fw_bcmdhd.bin : patcher.py dngl_sendpkt.bin dma_txfast_hook.bin text.bin patch.elf wlc_txfifo_hook.bin interrupt_enable_hook.bin setup_some_stuff_hook.bin bus_binddev_rom_hook.bin sub_1ECAB0_hook.bin
	./patcher.py

fw_bcmdhd.complete.bin : fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

clean:
	rm -f dngl_sendpkt.bin fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf patch.o dma_txfast_hook.bin data.bin text.bin wlc_txfifo_hook.bin interrupt_enable_hook.bin patch.elf dngl_sendpkt_alternative.o
