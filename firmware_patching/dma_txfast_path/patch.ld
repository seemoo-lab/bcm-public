SECTIONS
{
	.text.tr_pref_abort_hook 0x180F88: { KEEP(patch.o (.text.tr_pref_abort_hook)) }
	.text.tr_data_abort_hook 0x180FAA: { KEEP(patch.o (.text.tr_data_abort_hook)) }

	.text.interrupt_handler 0x180020: { KEEP(patch.o (.text.interrupt_handler)) }
	.text.before_before_initialize_memory_hook : { KEEP(patch.o (.text.before_before_initialize_memory_hook)) }

	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }
/*	.text : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) } */

/*	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) } */

	INCLUDE ../wrapper/wrapper.ld
}
