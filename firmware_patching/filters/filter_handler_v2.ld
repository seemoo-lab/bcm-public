SECTIONS
{
	.text 0x180050: { filter_handler_v2.o (.text .text.*) }
	.data : { filter_handler_v2.o (.data .data.*) }
	.bss : { filter_handler_v2.o (.bss .bss.*) }
	.rodata : { filter_handler_v2.o (.rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
