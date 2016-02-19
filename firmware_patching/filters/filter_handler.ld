SECTIONS
{
	.text 0x180050: { filter_handler.o (.text .text.*) }
	.data : { filter_handler.o (.data .data.*) }
	.bss : { filter_handler.o (.bss .bss.*) }
	.rodata : { filter_handler.o (.rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper.ld
}
