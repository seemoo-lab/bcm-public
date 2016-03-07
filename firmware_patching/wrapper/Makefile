CC=../../buildtools/arm-eabi-4.7/bin/arm-eabi-

all: wrapper.o wrapper_filter.o

wrapper.ld: ../include/wrapper.h Makefile
	# read the wrapper.h file and extract all lines starting with "extern", split them at "(" and "/" and then remove the "*" of a return type, then the function name is the second but last word and the address is the last word
	awk -F'[/(]' '/extern/ {gsub(/*/,"",$$1);print $$1 $$4}' ../include/wrapper.h | awk -F' ' '{print ".text." $$(NF-1) " " $$NF " : { ../wrapper/wrapper.o (.text." $$(NF-1) ") }"}' > wrapper.ld

wrapper.c: ../include/wrapper.h
	grep -v ^// ../include/wrapper.h | sed -e "/#/d" -e "/extern void \*/ s/;/ { return 0; }/ ; /extern void /! s/;/ { return 0; }/ ; s/);/) { ; }/" -e "s/extern //" > wrapper.c
	sed -e "/#/d" -e "/^extern void \*/ s/;/ { return 0; }/ ; /^extern void /! s/;/ { return 0; }/ ; s/);/) { ; }/" -e "s/^extern //" ../include/wrapper.h > wrapper.c

wrapper.o: wrapper.c wrapper.ld
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c wrapper.c -o wrapper.o
	# Change byte alignment to 2 byte:
	python2 ../../buildtools/elffile/adjalign.py wrapper.o wrapper.o
wrapper_filter.o: wrapper_filter.c wrapper_filter.ld
	$(CC)gcc -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -ffunction-sections -fdata-sections -c wrapper_filter.c -o wrapper_filter.o
	# Change byte alignment to 2 byte:
	python2 ../../buildtools/elffile/adjalign.py wrapper_filter.o wrapper_filter.o

clean: 
	rm -f wrapper.o wrapper_filter.o
