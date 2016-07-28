SECTIONS
{
	/* used to define the end off the free space used for our own code */
	.text.dummy_180800 0x180800: { KEEP(patch.o (.text.dummy_180800)) }

	/* The first section can be placed at 0x180020, following sections can be automatically placed after the first section */
	.text.wlc_channel_set_chanspec_hook 0x180020: { KEEP(patch.o (.text.wlc_channel_set_chanspec_hook)) }
	.text.wlc_set_chanspec_hook : { KEEP(patch.o (.text.wlc_set_chanspec_hook)) }
	.text.wlc_iovar_change_handler_hook : { KEEP(patch.o (.text.wlc_iovar_change_handler_hook)) }
	.text.wlc_set_var_chanspec_hook : { KEEP(patch.o (.text.wlc_set_var_chanspec_hook)) }
	.text.wlc_valid_chanspec_ext_hook : { KEEP(patch.o (.text.wlc_valid_chanspec_ext_hook)) }
	.text.phy_read_reg_hook : { KEEP(patch.o (.text.phy_read_reg_hook)) }
	.text.phy_write_reg_hook : { KEEP(patch.o (.text.phy_write_reg_hook)) }
	.text.write_radio_reg_hook : { KEEP(patch.o (.text.write_radio_reg_hook)) }

	/* All other sections that were not yet placed but referenced by a kept section are collected in the text section */
	.text : { patch.o (.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
