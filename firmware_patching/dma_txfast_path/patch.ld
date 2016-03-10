SECTIONS
{
	.text.console_size_r0 0x1EC344: { KEEP(patch.o (.text.console_size_r0)) }
	.text.console_size_r2 0x1EC360: { KEEP(patch.o (.text.console_size_r2)) }


	.text.interrupt_enable_hook 0x1804B4: { KEEP(patch.o (.text.interrupt_enable_hook)) }
	.text.wlc_ucode_download_hook : { KEEP(patch.o (.text.wlc_ucode_download_hook)) }
	.text.path_to_load_ucode_hook : { KEEP(patch.o (.text.path_to_load_ucode_hook)) }
	.text.interrupt_handler : { KEEP(patch.o (.text.interrupt_handler)) }
	.text.before_before_initialize_memory_hook : { KEEP(patch.o (.text.before_before_initialize_memory_hook)) }
/* unused hooks
	.text.dma_txfast_hook 0x1804B4: { KEEP(patch.o (.text.dma_txfast_hook)) }
	.text.wlc_txfifo_hook : { KEEP(patch.o (.text.wlc_txfifo_hook)) }
	.text.setup_some_stuff_hook : { KEEP(patch.o (.text.setup_some_stuff_hook)) }
	.text.bus_binddev_rom_hook : { KEEP(patch.o (.text.bus_binddev_rom_hook)) }
	.text.sub_1ECAB0_hook : { KEEP(patch.o (.text.sub_1ECAB0_hook)) }
	.text.function_with_huge_jump_table_hook : { KEEP(patch.o (.text.function_with_huge_jump_table_hook)) }
	.text.handle_ioctl_cmd_hook : { KEEP(patch.o (.text.handle_ioctl_cmd_hook)) }
*/
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }
	.text : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) }

	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) }
	INCLUDE ../wrapper/wrapper.ld
}
