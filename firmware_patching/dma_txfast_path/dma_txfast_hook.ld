SECTIONS
{
	.text 0x1804B4: { dma_txfast_hook.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	.text.dngl_sendpkt_alternative 0x180350: { dngl_sendpkt_alternative.o (.text.dngl_sendpkt_alternative) }
	INCLUDE ../wrapper/wrapper.ld
}
