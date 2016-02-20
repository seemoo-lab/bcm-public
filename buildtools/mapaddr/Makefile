mapaddr : mapaddr.o darm/armv7.o darm/armv7-tbl.o darm/darm.o darm/darm-tbl.o darm/thumb2.o darm/thumb2-decoder.o darm/thumb2-tbl.o darm/thumb.o darm/thumb-tbl.o
	gcc -o $@ $^ -lpcap

mapaddr.o : mapaddr.c
	gcc -std=c99 -Wall -Wno-unused-result -O0 -lpcap -D_BSD_SOURCE -DUSE_LIBPCAP -DMAP_FILE_NAME="firmware.map" -DROM_FILE_NAME="../../firmware_patching/dma_txfast_path/rom.bin" -DRAM_FILE_NAME="../../bootimg_src/firmware/fw_bcmdhd.orig.bin" -c mapaddr.c

trace.txt : trace.bin Makefile
	xxd -c 4 trace.bin | awk '{print $$2 $$3}' > trace.txt

darm/%.o: darm/%.c
	gcc -std=c99 -Wall -O2 -Wextra -Wno-missing-field-initializers -o $@ -c $^
