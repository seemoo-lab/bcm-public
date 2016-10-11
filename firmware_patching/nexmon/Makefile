SRCS=$(wildcard *.c) ucode_compressed.c
OBJS=$(SRCS:.c=.o)
Q=@

NEXMON_CHIP=CHIP_VER_BCM4339
NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_43_r639704
#NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_40_r581243
RAMSTART=0x180000

CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

ifeq ($(NEXMON_FW_VERSION), FW_VER_6_37_32_RC23_34_43_r639704)
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.6.37.32.RC23.34.43_r639704_hammerhead_mob30y.bin
else
	ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.orig.bin
endif

firmware: fw_bcmdhd.bin

driver: bcmdhd/bcmdhd.ko

all: firmware driver

include ucode_compression_code.mk

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../kernel/ M=$$PWD/bcmdhd -j2

%.o: %.c ../include/*.h
	$(Q)cat nexmon.generated.pre | gawk '{ if ($$3 != "$@") print; }' > tmp && mv tmp nexmon.generated.pre
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$@ \
		-fplugin-arg-nexmon-prefile=nexmon.generated.pre \
		-fplugin-arg-nexmon-chipver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_CHIP) | $(CC)gcc -E -x c - | tail -n 1` \
		-fplugin-arg-nexmon-fwver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_FW_VERSION) | $(CC)gcc -E -x c - | tail -n 1` \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

nexmon.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | \
		gawk '{ \
			if ($$2 == "PATCH") \
				print ".text." $$4 " " $$1 " : { KEEP(" $$3 " (.*." $$4 ")) }"; \
			else if ($$2 == "DUMMY") \
				print ".text.dummy." $$4 " " $$1 " : { " $$3 " (.*." $$4 ") }"; \
			else if ($$2 == "REGION") \
				print ".text." $$4 " : { KEEP(" $$3 " (.*." $$4 ")) } >" $$1; \
			else if ($$2 == "TARGETREGION") \
				print ".text." $$1 " : { " $$3 " (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) } >"  $$1; \
		}' > $@

nexmon.generated.mk: $(OBJS) FORCE
	$(Q)printf "fw_bcmdhd.bin: patch.elf FORCE\n" > $@
	$(Q)sort nexmon.generated.pre | \
		gawk '{ if ($$2 == "TARGETREGION") { "./getsectionaddr.sh .text." $$1 " patch.elf" | getline result; print $$0 " " result} else {print; }; }' | \
		gawk '{ \
			if ($$2 == "PATCH") \
				print "\t$$(Q)$$(CC)objcopy -O binary -j .text." $$4 " $$< section.generated.bin && dd if=section.generated.bin of=$$@ bs=1 conv=notrunc status=none seek=$$$$((" $$1 " - $(RAMSTART)))\n\t$$(Q)printf \"  PATCH " $$4 " @ " $$1 "\\n\""; \
			else if ($$2 == "REGION") \
				print "\t$$(Q)$$(CC)objcopy -O binary -j .text." $$1 " $$< section.generated.bin && dd if=section.generated.bin of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x" $$4 " - $(RAMSTART)))\t$$(Q)printf \"  REGION " $$1 " @ " $$4 "\\n\""; \
			else if ($$2 == "TARGETREGION") \
				print "\t$$(Q)$$(CC)objcopy -O binary -j .text." $$1 " $$< section.generated.bin && dd if=section.generated.bin of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x" $$4 " - $(RAMSTART)))\n\t$$(Q)printf \"  TARGETREGION " $$1 " @ " $$4 "\\n\""; \
		}' >> $@
	$(Q)printf "\nFORCE:\n" >> $@
	$(Q)gawk '!a[$$0]++' $@ > tmp && mv tmp $@

flashpatches.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | \
		gawk '{ \
			if ($$2 == "FLASHPATCH") \
				print ".text." $$4 " " $$1 " : { KEEP(" $$3 " (.*." $$4 ")) }"; \
		}' > $@

flashpatches.generated.mk: $(OBJS) FORCE
	cat nexmon.generated.pre | gawk ' \
		function htonl(a) { return rshift(and(a, 0xff000000), 24) + rshift(and(a, 0xff0000), 8) + lshift(and(a, 0xff00), 8) + lshift(and(a, 0xff), 24); } \
		BEGIN { \
			fp_data_base = 0x1D1800; \
			fp_data_end = fp_data_base; \
			fp_config_base = 0x207F0C; \
			fp_config_end = fp_config_base; \
			printf "fw_bcmdhd.bin: patch.elf FORCE\n"; \
		} \
		{ \
			if ($$2 == "FLASHPATCH") { \
				printf "\t$$(Q)$$(CC)objcopy -O binary -j .text." $$4 " $$< section.generated.bin && dd if=section.generated.bin of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", fp_data_end, 0x180000; \
				printf "\t$$(Q)printf %08x%08x%08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(strtonum($$1)), htonl(4), htonl(fp_data_end), fp_config_end, 0x180000; \
				printf "\t$$(Q)printf \"  FLASHPATCH %s @ %s\\n\"\n", $$4, $$1; \
				fp_data_end = fp_data_end + 8; \
				fp_config_end = fp_config_end + 12; \
			} \
		} \
		END { \
			printf "\t$$(Q)printf %08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(fp_data_end), 0x1d9ae0, 0x180000; \
			printf "\t$$(Q)printf \"  PATCH fp_data_end @ 0x%08x\\n\"\n", 0x1d9ae0; \
			printf "\t$$(Q)printf %08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(fp_config_base), 0x1ec610, 0x180000; \
			printf "\t$$(Q)printf \"  PATCH fp_config_base @ 0x%08x\\n\"\n", 0x1ec610; \
			printf "\t$$(Q)printf %08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(fp_config_end), 0x1ec60c, 0x180000; \
			printf "\t$$(Q)printf \"  PATCH fp_config_end @ 0x%08x\\n\"\n", 0x1ec60c; \
			printf "\t$$(Q)printf %08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(fp_config_base), 0x1ec8dc, 0x180000; \
			printf "\t$$(Q)printf \"  PATCH fp_config_base @ 0x%08x\\n\"\n", 0x1ec8dc; \
			printf "\t$$(Q)printf %08x | xxd -r -p | dd of=$$@ bs=1 conv=notrunc status=none seek=$$$$((0x%08x - 0x%08x))\n", htonl(fp_config_end), 0x1ec8d8, 0x180000; \
			printf "\t$$(Q)printf \"  PATCH fp_config_end @ 0x%08x\\n\"\n", 0x1ec8d8; \
			printf "\n\nFORCE:\n" \
		}' > flashpatches.generated.mk

wrapper.o: ../include/wrapper.c
	$(Q)make -C ../wrapper \
		NEXMON_CHIP=$(NEXMON_CHIP) \
		NEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		WRAPPER_PRE_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/nexmon.generated.pre \
		WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/$@ \
		Q=$(Q)

patch.elf: patch.ld patch.o wrapper.o nexmon.generated.ld flashpatches.generated.ld $(OBJS)
	$(Q)$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M

fw_bcmdhd.bin: patch.elf $(ORIG_FW) nexmon.generated.mk flashpatches.generated.mk
	cp $(ORIG_FW) $@
	$(Q)make -f flashpatches.generated.mk Q=$(Q)
	$(Q)make -f nexmon.generated.mk Q=$(Q)

fw_bcmdhd.complete.bin : fw_bcmdhd.bin ../../bootimg_src/firmware/rom.bin
	dd if=../../bootimg_src/firmware/rom.bin status=none of=$@ bs=1
	dd if=fw_bcmdhd.bin of=$@ bs=1 status=none seek=1572864

fw_bcmdhd.complete.elf : fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary $< $@
	$(CC)objcopy --set-section-flags .data=code $@

check-nexmon-setup-env:
ifndef NEXMON_SETUP_ENV
	$(error run 'source setup_env.sh' first)
endif

install-firmware: fw_bcmdhd.bin
	adb shell 'su -c "mount -o rw,remount /system"' && \
	adb push $< /sdcard/ && \
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /vendor/firmware/fw_bcmdhd.bin"'
	adb shell 'su -c "ifconfig wlan0 down"'

backup-firmware: FORCE
	adb shell 'su -c "cp /vendor/firmware/fw_bcmdhd.bin /sdcard/fw_bcmdhd.orig.bin"'
	adb pull /sdcard/fw_bcmdhd.orig.bin

install-backup: fw_bcmdhd.orig.bin
	adb shell 'su -c "mount -o rw,remount /system"' && \
	adb push $< /sdcard/ && \
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /vendor/firmware/fw_bcmdhd.bin"'
	adb shell 'su -c "ifconfig wlan0 down && ifconfig wlan0 up"'

clean-firmware: FORCE
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf *.o patch.elf ucode_compressed.c ucode_compressed.bin *.generated.* wrapper.ld
	make -C ../wrapper clean

clean: clean-firmware
	make -C ../../kernel/ M=$$PWD/bcmdhd clean

FORCE:
