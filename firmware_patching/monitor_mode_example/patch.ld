SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

	/* we overwrite the existing wlc_bmac_recv function with our hook */
	.text.wlc_bmac_recv_hook 0x1AAD98: { KEEP(patch.o (.text.wlc_bmac_recv_hook)) }

	/* All other sections that were not yet placed but referenced by a kept section are collected in the text section */
	.text 0x180020: { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
