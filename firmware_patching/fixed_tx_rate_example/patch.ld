SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

    .text.wlc_d11hdrs_hook 0x180020 : { KEEP(patch.o (.text.wlc_d11hdrs_hook)) }

	/* The first section can be placed at 0x180020, following sections can be automatically placed after the first section */
	/* .text.function_1 0x180020: { KEEP(patch.o (.text.function_1)) } */
	/* .text.function_2 : { KEEP(patch.o (.text.function_2)) } */

	/* All other sections that were not yet placed but referenced by a kept section are collected in the text section */
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
