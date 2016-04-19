CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: bcmdhd/bcmdhd.ko fw_bcmdhd.bin

bcmdhd/bcmdhd.ko: FORCE
	if [ "$$ARCH" = "arm" ]; then make -C ../../kernel/ M=$$PWD/bcmdhd -j2; else echo "ERR: run 'source setup_env.sh' first"; fi

fw_bcmdhd.bin: patcher.py patch.elf text.bin handle_exceptions.bin tr_pref_abort_hook.bin tr_data_abort_hook.bin before_before_initialize_memory_hook.bin si_update_chipcontrol_shm_hook.bin
	./patcher.py

patch.o: patch.c ../include/*.h
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -c $< -o $@

../wrapper/wrapper.o: ../include/wrapper.h
	make -C ../wrapper

patch.elf: patch.ld patch.o ../wrapper/wrapper.o
	$(CC)ld -T $< --entry=dma_txfast_hook -o $@ --gc-sections --print-gc-sections -M

tr_pref_abort_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.tr_pref_abort_hook $< $@

tr_data_abort_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.tr_data_abort_hook $< $@

handle_exceptions.bin: patch.elf
	$(CC)objcopy -O binary -j .text.handle_exceptions $< $@

before_before_initialize_memory_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.before_before_initialize_memory_hook $< $@

si_update_chipcontrol_shm_hook.bin: patch.elf
	$(CC)objcopy -O binary -j .text.si_update_chipcontrol_shm_hook $< $@

text.bin: patch.elf
	$(CC)objcopy -O binary -j .text $< $@

fw_bcmdhd.complete.bin : fw_bcmdhd.bin rom.bin
	dd if=rom.bin of=fw_bcmdhd.complete.bin bs=1
	dd if=fw_bcmdhd.bin of=fw_bcmdhd.complete.bin bs=1 seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf
	$(CC)objcopy --set-section-flags .data=code fw_bcmdhd.complete.elf

clean:
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf patch.o patch.elf text.bin handle_exceptions.bin tr_pref_abort_hook.bin tr_data_abort_hook.bin before_before_initialize_memory_hook.bin si_update_chipcontrol_shm_hook.bin
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE: