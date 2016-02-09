SECTIONS
{
	.text.dma_txfast_hook 0x1804B4: { KEEP(patch.o (.text.dma_txfast_hook)) }
	.text.wlc_txfifo_hook : { KEEP(patch.o (.text.wlc_txfifo_hook)) }
	.text.interrupt_enable_hook : { KEEP(patch.o (.text.interrupt_enable_hook)) }
	.text.setup_some_stuff_hook : { KEEP(patch.o (.text.setup_some_stuff_hook)) }
	.text.bus_binddev_rom_hook : { KEEP(patch.o (.text.bus_binddev_rom_hook)) }
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }
	.text : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) }

	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) }
	INCLUDE ../wrapper/wrapper.ld
}
