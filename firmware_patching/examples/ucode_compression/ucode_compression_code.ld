.text.wlc_ucode_write_compressed 0x1fd820 : { KEEP(patch.o (.text.wlc_ucode_write_compressed)) }
.text.ucode_compression_code : { KEEP(patch.o ( \
	.text.wlc_bmac_write_objmem_byte \
	.text.gen_huffman_table \
	.text.tinflate_partial \
	.text.tinflate_write_objmem \
	.text.tinflate_read_objmem \
	.rodata.codelen_order.* \
	.data.ucode_compressed_bin \
	.data.ucode_compressed_bin_len \
	)) }