CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: wrapper.o

wrapper.c: ../include/wrapper.h
	sed -e "s/extern //" -e "/#/d" -e "s/;/ { return 0; }/" ../include/wrapper.h > wrapper.c

wrapper.o: wrapper.c wrapper.ld
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c wrapper.c -o wrapper.o
	# Change byte alignment to 2 byte:
	python2 ../../buildtools/elffile/adjalign.py wrapper.o wrapper.o

clean: 
	rm -f wrapper.o wrapper.c
