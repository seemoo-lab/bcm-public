FUNCTIONS += ucode_compression_code wlc_ucode_write_compressed
BCMDHDFWPATH =../../bootimg_src/firmware/fw_bcmdhd.orig.bin

ucode.bin: $(BCMDHDFWPATH) Makefile
	dd if=$< of=$@ bs=1 skip=$$((0x1fd820-0x180000)) count=$$((0xb2e0))

ucode_compressed.bin: ucode.bin
	cat $< | zlib-flate -compress > $@

ucode_compressed.c: ucode_compressed.bin
	printf "#pragma NEXMON targetregion \"ucode\"\n\n" > $@
	xxd -i $< >> $@