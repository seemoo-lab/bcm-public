SRCS=$(wildcard *.c) ucode_compressed.c
OBJS=$(SRCS:.c=.o)
Q=@

NEXMON_CHIP=CHIP_VER_BCM4339
NEXMON_CHIP_NUM=`./getdefine.sh $(NEXMON_CHIP)`
NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_43_r639704
NEXMON_FW_VERSION_NUM=`./getdefine.sh $(NEXMON_FW_VERSION)`
ORIG_FW=../../bootimg_src/firmware/fw_bcmdhd.6.37.32.RC23.34.43_r639704_hammerhead_mob30y.bin
ROM_FILE=../../bootimg_src/firmware/rom.bin
ROMSTART=0x0
RAMSTART=0x180000
PATCHSTART=0x180020
ROMSIZE=0xA0000
RAMSIZE=0xC0000
PATCHSIZE=0x7e0
UCODESTART=0x1fd82c
UCODESIZE=0xb2e0
FP_DATA_END_PTR=0x1d9ae0
FP_CONFIG_BASE_PTR_1=0x1ec610
FP_CONFIG_END_PTR_1=0x1ec60c
FP_CONFIG_BASE_PTR_2=0x1ec8dc
FP_CONFIG_END_PTR_2=0x1ec8d8
FP_CONFIG_BASE=0x207F0C
FP_CONFIG_SIZE=0xc00
FP_DATA_BASE=0x1D1800

CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

firmware: fw_bcmdhd.bin

driver: bcmdhd/bcmdhd.ko

all: firmware driver

bcmdhd/bcmdhd.ko: check-nexmon-setup-env
	make -C ../../kernel/ M=$$PWD/bcmdhd -j2

%.o: %.c ../include/*.h
	$(Q)cat nexmon.generated.pre | gawk '{ if ($$3 != "$@") print; }' > tmp && mv tmp nexmon.generated.pre
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$@ \
		-fplugin-arg-nexmon-prefile=nexmon.generated.pre \
		-fplugin-arg-nexmon-chipver=$(NEXMON_CHIP_NUM) \
		-fplugin-arg-nexmon-fwver=$(NEXMON_FW_VERSION_NUM) \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

nexmon.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | gawk -f nexmon.ld.awk > $@

nexmon.generated.mk: $(OBJS) FORCE
	$(Q)printf "fw_bcmdhd.bin: patch.elf FORCE\n" > $@
	$(Q)sort nexmon.generated.pre | gawk -f nexmon.mk.1.awk | gawk -v ramstart=$(RAMSTART) -f nexmon.mk.2.awk >> $@
	$(Q)printf "\nFORCE:\n" >> $@
	$(Q)gawk '!a[$$0]++' $@ > tmp && mv tmp $@

flashpatches.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | \
		gawk -f flashpatches.ld.awk > $@

flashpatches.generated.mk: $(OBJS) FORCE
	cat nexmon.generated.pre | gawk \
		-v fp_data_base=$(FP_DATA_BASE) \
		-v fp_config_base=$(FP_CONFIG_BASE) \
		-v fp_data_end_ptr=$(FP_DATA_END_PTR) \
		-v fp_config_base_ptr_1=$(FP_CONFIG_BASE_PTR_1) \
		-v fp_config_end_ptr_1=$(FP_CONFIG_END_PTR_1) \
		-v fp_config_base_ptr_2=$(FP_CONFIG_BASE_PTR_2) \
		-v fp_config_end_ptr_2=$(FP_CONFIG_END_PTR_2) \
		-v ramstart=$(RAMSTART) \
		-f flashpatches.mk.awk > flashpatches.generated.mk

memory.generated.ld: FORCE
	$(Q)printf "rom : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(ROMSTART) $(ROMSIZE) > $@
	$(Q)printf "ram : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(RAMSTART) $(RAMSIZE) >> $@
	$(Q)printf "patch : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(PATCHSTART) $(PATCHSIZE) >> $@
	$(Q)printf "ucode : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(UCODESTART) $$(($(UCODESIZE)-$(FP_CONFIG_SIZE))) >> $@
	$(Q)printf "fpconfig : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(FP_CONFIG_BASE) $(FP_CONFIG_SIZE) >> $@

wrapper.o: ../include/wrapper.c
	$(Q)make -C ../wrapper \
		NEXMON_CHIP=$(NEXMON_CHIP) \
		NEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		WRAPPER_PRE_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/nexmon.generated.pre \
		WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon/$@ \
		Q=$(Q)

patch.elf: patch.ld wrapper.o nexmon.generated.ld memory.generated.ld $(OBJS)
	$(Q)$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M

flashpatches.elf: flashpatches.ld wrapper.o flashpatches.generated.ld memory.generated.ld $(OBJS)
	$(Q)$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M

fw_bcmdhd.bin: patch.elf flashpatches.elf $(ORIG_FW) nexmon.generated.mk flashpatches.generated.mk
	cp $(ORIG_FW) $@
	$(Q)make -f flashpatches.generated.mk Q=$(Q)
	$(Q)make -f nexmon.generated.mk Q=$(Q)

fw_bcmdhd.complete.bin: fw_bcmdhd.bin $(ROM_FILE)
	dd if=$(ROM_FILE) of=$@ bs=1 status=none conv=notrunc seek=$$(($(ROMSTART)))
	dd if=fw_bcmdhd.bin of=$@ bs=1 status=none conv=notrunc seek=$$(($(RAMSTART)))

fw_bcmdhd.complete.elf: fw_bcmdhd.complete.bin
	$(CC)objcopy -O elf32-littlearm -Barm -I binary $< $@
	$(CC)objcopy --set-section-flags .data=code $@

###################################################################
# ucode compression related
###################################################################

ucode.bin: $(ORIG_FW) Makefile
	dd if=$< of=$@ bs=1 skip=$$(($(UCODESTART)-$(RAMSTART))) count=$$(($(UCODESIZE)))

ucode_compressed.bin: ucode.bin
	cat $< | zlib-flate -compress > $@

ucode_compressed.c: ucode_compressed.bin
	printf "#pragma NEXMON targetregion \"ucode\"\n\n" > $@
	xxd -i $< >> $@

###################################################################

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
