SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

	/* write deflation functions and compressed ucode to address of original ucode */
	INCLUDE tinflate.ld

	.text.wlc_ucode_write_alternative 0x180020 : { KEEP(patch.o (.text.wlc_ucode_write_alternative)) }

	/* All other sections that were not yet placed but referenced by a kept section are collected in the text section */
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
