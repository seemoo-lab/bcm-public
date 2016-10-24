CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

NEXMON_CHIP=CHIP_VER_BCM4339
NEXMON_FW_VERSION=FW_VER_6_37_32_RC23_34_40_r581243
NEXMON_MARCH=armv7-r

WRAPPER_PRE_FILE=wrapper.generated.pre
WRAPPER_OBJ_FILE=$(NEXMON_FIRMWARE_PATCHING)/wrapper/wrapper.o

all: $(WRAPPER_OBJ_FILE)

$(WRAPPER_OBJ_FILE): ../include/wrapper.c
	$(Q)cat $(WRAPPER_PRE_FILE) | gawk '{ if ($$3 != "$@") print; }' > tmp && mv tmp $(WRAPPER_PRE_FILE)
	$(Q)$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$(WRAPPER_OBJ_FILE) \
		-fplugin-arg-nexmon-prefile=$(WRAPPER_PRE_FILE) \
		-fplugin-arg-nexmon-chipver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_CHIP) | $(CC)gcc -E -x c - | tail -n 1` \
		-fplugin-arg-nexmon-fwver=`printf "#include \"../include/firmware_version.h\"\n%s\n" $(NEXMON_FW_VERSION) | $(CC)gcc -E -x c - | tail -n 1` \
		-DNEXMON_CHIP=$(NEXMON_CHIP) \
		-DNEXMON_FW_VERSION=$(NEXMON_FW_VERSION) \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=$(NEXMON_MARCH) -ffunction-sections -fdata-sections -I../include -c $< -o $@

clean: 
	rm -f wrapper.o wrapper.ld
