SECTIONS
{
	.text 0x180050: { filter.o (.text .text.*) }
	.data : { filter.o (.data .data.*) }
	.bss : { filter.o (.bss .bss.*) }
	.rodata : { filter.o (.rodata .rodata.*) }

	INCLUDE ../wrapper/wrapper_filter.ld
}
