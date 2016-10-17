MEMORY
{
	rom : ORIGIN = 0x0, LENGTH = 640K
	ram : ORIGIN = 0x180000, LENGTH = 768K
	patch : ORIGIN = 0x181000, LENGTH = 0xb7c
	ucode : ORIGIN = 0x20b4cc, LENGTH = 0xc918
	fpconfig : ORIGIN = 0x217de4, LENGTH = 0xc00
}
/* 0x217de4*/
/* patch : ORIGIN = 0x1800dc, LENGTH = 36 */

SECTIONS
{
	INCLUDE flashpatches.generated.ld
	INCLUDE nexmon.generated.ld
}
