SECTIONS
{
	.text 0x1AAD98: { wlc_bmac_recv.o (.text .text.*) }
    .data : { wlc_bmac_recv.o (.data .data.*) }
    .bss : { wlc_bmac_recv.o (.bss .bss.*) }
    .rodata : { wlc_bmac_recv.o (.rodata .rodata.*) }

    .text.dma_rx 0x8c69c : { wrapper.o (.text.dma_rx) }
    .text.dma_rxfill 0x8c6cc : { wrapper.o (.text.dma_rxfill) }
    .text.dngl_sendpkt 0x182750 : { wrapper.o (.text.dngl_sendpkt) }
    .text.printf 0x126f0 : { wrapper.o (.text.printf) }
    .text.wlc_mctrl_write 0x4df60 : { wrapper.o (.text.wlc_mctrl_write) }
    .text.wlc_bmac_mctrl 0x4F080: { wrapper.o (.text.wlc_bmac_mctrl)}
}
