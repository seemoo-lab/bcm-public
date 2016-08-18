SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

	/* replaces the existing tr_reset function */
	.text.tr_reset_hook 0x180880: { KEEP(patch.o (.text.tr_reset_hook)) }
	
	/* replaces the existing tr_pref_abort function */
	.text.tr_pref_abort_hook 0x180F88: { KEEP(patch.o (.text.tr_pref_abort_hook)) }
		
	/* replaces the existing handle_exceptions function */
	.text.handle_exceptions 0x181032: { KEEP(patch.o (.text.handle_exceptions)) }
	
	/* used to define the end the replaced handle_exceptions function */
	.text.dummy_181098 0x181098: { KEEP(patch.o (.text.dummy_181098)) }

	.text.before_before_initialize_memory_hook 0x180020: { KEEP(patch.o (.text.before_before_initialize_memory_hook)) }

	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../../wrapper/wrapper.ld
}
