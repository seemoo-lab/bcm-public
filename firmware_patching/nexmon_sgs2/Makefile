SRCS=$(wildcard *.c) ucode_compressed.c
NEXMONSRCS=$(wildcard ../nexmon/*.c)
OBJS=$(SRCS:.c=.o) $(notdir $(NEXMONSRCS:.c=.o))
Q=

NEXMON_CHIP=CHIP_VER_BCM4330
NEXMON_CHIP_NUM=`../nexmon/getdefine.sh $(NEXMON_CHIP)`
NEXMON_FW_VERSION=FW_VER_5_90_100_41
NEXMON_FW_VERSION_NUM=`../nexmon/getdefine.sh $(NEXMON_FW_VERSION)`
NEXMON_MARCH=armv7-m
ORIG_FW=/nexmon/bcm-ida-files/bcm4330/cyanogenmod/bcmdhd_sta.bin
ROM_FILE=../../bootimg_src/firmware/rom.bin
ROMSTART=0x800000
RAMSTART=0x0
ROMSIZE=0x80000
RAMSIZE=0x48000
PATCHSTART=0x3436C
PATCHSIZE=0x800
UCODESTART=0x2D830
UCODESIZE=0x8B82
FP_DATA_END_PTR=0x24060
FP_CONFIG_BASE_PTR_1=0xB370
FP_CONFIG_END_PTR_1=0xB374
FP_CONFIG_BASE_PTR_2=0x27F2C
FP_CONFIG_END_PTR_2=0x27F30
FP_CONFIG_BASE=0x341EC
FP_CONFIG_SIZE=0x180
FP_DATA_BASE=0x24E00

CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

firmware: fw_bcmdhd.bin

all: firmware

%.o: ../nexmon/%.c ../include/*.h
	$(Q)cat nexmon.generated.pre | gawk '{ if ($$3 != "$@") print; }' > tmp && mv tmp nexmon.generated.pre
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$@ \
		-fplugin-arg-nexmon-prefile=nexmon.generated.pre \
		-fplugin-arg-nexmon-chipver=$(NEXMON_CHIP_NUM) \
		-fplugin-arg-nexmon-fwver=$(NEXMON_FW_VERSION_NUM) \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=$(NEXMON_MARCH) -ffunction-sections -fdata-sections -I../include -c $< -o $@


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
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=$(NEXMON_MARCH) -ffunction-sections -fdata-sections -I../include -c $< -o $@

nexmon.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | gawk -f ../nexmon/nexmon.ld.awk > $@

nexmon.generated.mk: $(OBJS) FORCE
	$(Q)printf "fw_bcmdhd.bin: patch.elf FORCE\n" > $@
	$(Q)sort nexmon.generated.pre | gawk -f ../nexmon/nexmon.mk.1.awk | gawk -v ramstart=$(RAMSTART) -f ../nexmon/nexmon.mk.2.awk >> $@
	$(Q)printf "\nFORCE:\n" >> $@
	$(Q)gawk '!a[$$0]++' $@ > tmp && mv tmp $@

flashpatches.generated.ld: $(OBJS) FORCE
	$(Q)sort nexmon.generated.pre | \
		gawk -f ../nexmon/flashpatches.ld.awk > $@

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
		-f ../nexmon/flashpatches.mk.awk > flashpatches.generated.mk

memory.generated.ld: FORCE
	$(Q)printf "rom : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(ROMSTART) $(ROMSIZE) > $@
	$(Q)printf "ram : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(RAMSTART) $(RAMSIZE) >> $@
	$(Q)printf "patch : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(PATCHSTART) $(PATCHSIZE) >> $@
	$(Q)printf "ucode : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(UCODESTART) $$(($(UCODESIZE)-$(FP_CONFIG_SIZE)-$(PATCHSIZE))) >> $@
	$(Q)printf "fpconfig : ORIGIN = 0x%08x, LENGTH = 0x%08x\n" $(FP_CONFIG_BASE) $(FP_CONFIG_SIZE) >> $@

wrapper.o: ../include/wrapper.c
	$(Q)make -C ../wrapper \
		NEXMON_CHIP=$(NEXMON_CHIP) \
		NEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		WRAPPER_PRE_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon_sgs2/nexmon.generated.pre \
		WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/nexmon_sgs2/$@ \
		NEXMON_MARCH=$(NEXMON_MARCH) \
		Q=$(Q)

patch.elf: patch.ld wrapper.o nexmon.generated.ld flashpatches.generated.ld memory.generated.ld $(OBJS)
	$(Q)$(CC)ld -T $< -o $@ --gc-sections --print-gc-sections -M


fw_bcmdhd.bin: patch.elf $(ORIG_FW) nexmon.generated.mk flashpatches.generated.mk
	cp $(ORIG_FW) $@
	$(Q)make -f nexmon.generated.mk Q=$(Q)
	$(Q)make -f flashpatches.generated.mk Q=$(Q)

###################################################################
# ucode compression related
###################################################################

ucode.bin: $(ORIG_FW) Makefile
	../../buildtools/ucode_extractor/ucodeext -r $< -b $(UCODESTART) -l $(UCODESIZE) -o $@

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
	adb shell 'su -c "cp /sdcard/fw_bcmdhd.bin /system/etc/wifi/bcmdhd_sta.bin"'
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
	rm -f fw_bcmdhd.bin fw_bcmdhd.complete.bin fw_bcmdhd.complete.elf *.o patch.elf ucode_compressed.c ucode_compressed.bin *.generated.* wrapper.ld ucode.bin
	make -C ../wrapper clean

clean: clean-firmware

FORCE:
