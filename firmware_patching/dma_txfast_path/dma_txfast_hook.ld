SECTIONS
{
	.text 0x1804B4: { dma_txfast_hook.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
