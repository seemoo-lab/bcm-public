SECTIONS
{
	.text.dma_txfast_hook 0x1804B4: { patch.o (.text.dma_txfast_hook) }
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }
	.text : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) }

	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) }
	INCLUDE ../wrapper/wrapper.ld
}
