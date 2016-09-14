CC=../../buildtools/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-

all: wrapper.o

wrapper.o: ../include/wrapper.c
	$(CC)gcc \
		-fplugin=../../buildtools/gcc-nexmon-plugin/nexmon.so \
		-fplugin-arg-nexmon-objfile=$(NEXMON_FIRMWARE_PATCHING)/wrapper/wrapper.o \
		-fplugin-arg-nexmon-ldfile=wrapper.ld \
		-Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv7-r -ffunction-sections -fdata-sections -I../include -c $< -o $@

clean: 
	rm -f wrapper.o wrapper.ld
