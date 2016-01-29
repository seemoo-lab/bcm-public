SECTIONS
{
	.text 0x1804B4: { dma_txfast_hook.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	.data : { bdc_ethernet_ipv6_udp_header_array.o (.data.*) }

	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) }
	INCLUDE ../wrapper/wrapper.ld
}
