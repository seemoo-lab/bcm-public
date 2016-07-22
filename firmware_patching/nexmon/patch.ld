SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

	/* used to define the end off the free space used for our wlc_bmac_recv_hook */
	.text.dummy_1AAEB4 0x1AAEB4: { KEEP(patch.o (.text.dummy_1AAEB4)) }

	/* we overwrite the existing wlc_bmac_recv function with our hook */
	.text.wlc_bmac_recv_hook 0x1AAD98: { KEEP(patch.o (.text.wlc_bmac_recv_hook)) }

	/* this dma_attach_hook resets the rxextheadroom parameter to increase the header size to store the bdc and radiotap headers */
	.text.dma_attach_hook 0x180020: { KEEP(patch.o (.text.dma_attach_hook)) }

    .text.sdio_handler : { KEEP(patch.o (.text.sdio_handler)) }

	/* All other sections that were not yet placed but referenced by a kept section are collected in the text section */
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
