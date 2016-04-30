SECTIONS
{
    /* used to define the end off the free space used for our own code */
    .text.dummy_180800 0x180800: { KEEP(wlc_bmac_recv.o (.text.dummy_180800)) }
	.text.wlc_bmac_recv 0x1AAD98: { KEEP(wlc_bmac_recv.o (.text.wlc_bmac_recv)) }
	.text.nexmon_filter 0x180050: { KEEP(wlc_bmac_recv.o (.text.nexmon_filter)) }
/*    .text.towards_dma_txfast_hook 0x180050: { KEEP(wlc_bmac_recv.o (.text.towards_dma_txfast_hook)) } */
    .text.sdio_handler : { KEEP(wlc_bmac_recv.o (.text.sdio_handler)) }
	.text : { wlc_bmac_recv.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
