SECTIONS
{
	.text 0x1AAD98: { wlc_bmac_recv.o (.text .text.*) }
	.data : { wlc_bmac_recv.o (.data .data.*) }
	.bss : { wlc_bmac_recv.o (.bss .bss.*) }
	.rodata : { wlc_bmac_recv.o (.rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
