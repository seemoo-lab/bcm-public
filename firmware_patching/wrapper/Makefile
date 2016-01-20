CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: wrapper.o

wrapper.o: wrapper.c
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c wrapper.c -o wrapper.o
	python2 ../../buildtools/elffile/adjalign.py wrapper.o wrapper.o

clean: 
	rm -f wrapper.o
