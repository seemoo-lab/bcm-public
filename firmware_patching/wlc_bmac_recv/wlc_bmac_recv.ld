SECTIONS
{
	.text.wlc_bmac_recv 0x1AAD98: { KEEP(wlc_bmac_recv.o (.text.wlc_bmac_recv)) }
	.text.nexmon_filter 0x180050: { KEEP(wlc_bmac_recv.o (.text.nexmon_filter)) }
	.text : { wlc_bmac_recv.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
