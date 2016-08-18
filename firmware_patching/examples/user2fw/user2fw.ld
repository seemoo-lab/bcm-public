SECTIONS
{
	.text 0x180050: { user2fw.o (.text .text.*) }
	.data : { user2fw.o (.data .data.*) }
	.bss : { user2fw.o (.bss .bss.*) }
	.rodata : { user2fw.o (.rodata .rodata.*) }

	INCLUDE ../../wrapper/wrapper.ld
}
