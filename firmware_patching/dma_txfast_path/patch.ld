SECTIONS
{
	.text.tr_reset_hook 0x180880: { KEEP(patch.o (.text.tr_reset_hook)) }
	.text.tr_pref_abort_hook 0x180F88: { KEEP(patch.o (.text.tr_pref_abort_hook)) }
	.text.tr_data_abort_hook 0x180FAA: { KEEP(patch.o (.text.tr_data_abort_hook)) }
	.text.handle_exceptions 0x181032: { KEEP(patch.o (.text.handle_exceptions)) }
	.text.enable_irq_interrupt 0x181098: { KEEP(patch.o (.text.enable_irq_interrupt)) }

	.text.before_before_initialize_memory_hook 0x180020: { KEEP(patch.o (.text.before_before_initialize_memory_hook)) }

	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }
/*	.text : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) } */

/*	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) } */

	INCLUDE ../wrapper/wrapper.ld
}
