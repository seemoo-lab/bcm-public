#/**************************************************************************
#*                                                                         *
#*          ###########   ###########   ##########    ##########           *
#*         ############  ############  ############  ############          *
#*         ##            ##            ##   ##   ##  ##        ##          *
#*         ##            ##            ##   ##   ##  ##        ##          *
#*         ###########   ####  ######  ##   ##   ##  ##    ######          *
#*          ###########  ####  #       ##   ##   ##  ##    #    #          *
#*                   ##  ##    ######  ##   ##   ##  ##    #    #          *
#*                   ##  ##    #       ##   ##   ##  ##    #    #          *
#*         ############  ##### ######  ##   ##   ##  ##### ######          *
#*         ###########    ###########  ##   ##   ##   ##########           *
#*                                                                         *
#*            S E C U R E   M O B I L E   N E T W O R K I N G              *
#*                                                                         *
#* Warning:                                                                *
#*                                                                         *
#* Our software may damage your hardware and may void your hardware’s      *
#* warranty! You use our tools at your own risk and responsibility!        *
#*                                                                         *
#* License:                                                                *
#* Copyright (c) 2015 NexMon Team                                          *
#*                                                                         *
#* Permission is hereby granted, free of charge, to any person obtaining   *
#* a copy of this software and associated documentation files (the         *
#* "Software"), to deal in the Software without restriction, including     *
#* without limitation the rights to use, copy, modify, merge, publish,     *
#* distribute copies of the Software, and to permit persons to whom the    *
#* Software is furnished to do so, subject to the following conditions:    *
#*                                                                         *
#* The above copyright notice and this permission notice shall be included *
#* in all copies or substantial portions of the Software.                  *
#*                                                                         *
#* Any use of the Software which results in an academic publication or     *
#* other publication which includes a bibliography must include a citation *
#* to the author's publication "M. Schulz, D. Wegemer and M. Hollick.      *
#* NexMon: A Cookbook for Firmware Modifications on Smartphones to Enable  *
#* Monitor Mode.".                                                         *
#*                                                                         *
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS *
#* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              *
#* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
#* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
#* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,    *
#* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE       *
#* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                  *
#*                                                                         *
#**************************************************************************/

B43DASM=../../buildtools/b43/disassembler/b43-dasm
B43ASM=../../buildtools/b43/assembler/b43-asm
B43BEAUTIFIER=../../buildtools/b43/debug/b43-beautifier
B43DEFS=../../buildtools/b43/debug/include/
BCMDHDFWPATH=../../bootimg_src/firmware/fw_bcmdhd.orig.bin

ucode.bin: $(BCMDHDFWPATH) Makefile
	dd if=$< of=$@ bs=1 skip=$$((0x1fd820-0x180000)) count=$$((0xb2e0))

ucode.asm: ucode.bin
	$(B43DASM) $< $@ --arch 15 --format raw-le32

ucode-beautified.asm: ucode.asm
	$(B43BEAUTIFIER) --asmfile $< --defs $(B43DEFS) > $@

ucode-commented.asm: ucode-beautified.asm Makefile
	awk 'BEGIN{FS=", |\t"} { \
			if (/^L/) { print $0 } \
			else if (/\tadd\.\t/) { printf ("%-50s\t/* add, set carry: %s = %s + %s SaveCarry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\taddc\.\t/) { printf ("%-50s\t/* add with carry, set carry: %s = %s + %s + Carry SaveCarry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\taddc\t/) { printf ("%-50s\t/* add with carry: %s = %s + %s + Carry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tadd\t/) { printf ("%-50s\t/* add: %s = %s + %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsub\.\t/) { printf ("%-50s\t/* sub, set carry: %s = %s - %s SaveCarry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsubc\.\t/) { printf ("%-50s\t/* sub with carry, set carry: %s = %s - %s - Carry SaveCarry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsubc\t/) { printf ("%-50s\t/* sub with carry: %s = %s - %s - Carry */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsub\t/) { printf ("%-50s\t/* sub: %s = %s - %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tmul\t/) { printf ("%-50s\t/* multiply: spr06d = lsb; %s = msb */\n", $$0, $$5) } \
			else if (/\tjmp\t/) { printf ("%-50s\t/* uncond. jump: PC = %s */\n", $$0, $$3) } \
			else if (/\tjand\t/) { printf ("%-50s\t/* jump if binary AND: if(%s & %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjnand\t/) { printf ("%-50s\t/* jump if not binary AND: if(!(%s & %s)) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjs\t/) { printf ("%-50s\t/* jump if all bits set: if((%s & %s) == %s) PC = %s */\n", $$0, $$3, $$4, $$3, $$5) } \
			else if (/\tjns\t/) { printf ("%-50s\t/* jump if not all bits set: if((%s & %s) != %s) PC = %s */\n", $$0, $$3, $$4, $$3, $$5) } \
			else if (/\tje\t/) { printf ("%-50s\t/* jump if equal: if(%s == %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjne\t/) { printf ("%-50s\t/* jump if not equal: if(%s != %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjls\t/) { printf ("%-50s\t/* jump if less (signed): if(%s < %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjges\t/) { printf ("%-50s\t/* jump if greater or equal (signed): if(%s >= %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjge\t/) { printf ("%-50s\t/* jump if greater or equal: if(%s >= %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjgs\t/) { printf ("%-50s\t/* jump if greater (signed): if(%s > %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjles\t/) { printf ("%-50s\t/* jump if less or equal (signed): if(%s <= %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjle\t/) { printf ("%-50s\t/* jump if less or equal: if(%s <= %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjl\t/) { printf ("%-50s\t/* jump if less: if(%s < %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjg\t/) { printf ("%-50s\t/* jump if greater: if(%s > %s) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjdnz\t/) { printf ("%-50s\t/* jump if diff is <= 0, no carry: if(nc(%s - %s) <= 0) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjdn\t/) { printf ("%-50s\t/* jump if diff is < 0, no carry: if(nc(%s - %s) < 0) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjdpz\t/) { printf ("%-50s\t/* jump if diff is >= 0, no carry: if(nc(%s - %s) >= 0) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tjdp\t/) { printf ("%-50s\t/* jump if diff is > 0, no carry: if(nc(%s - %s) > 0) PC = %s */\n", $$0, $$3, $$4, $$5) } \
			else if (/\tcalls\t/) { printf ("%-50s\t/* Push PC to stack, call function: PC -> stack; PC = %s */\n", $$0, $$3) } \
			else if (/\tcall\t/) { printf ("%-50s\t/* Store PC to link register, call function: %s = PC; PC = %s */\n", $$0, $$3, $$4) } \
			else if (/\trets\t/) { printf ("%-50s\t/* pop PC from stack, return from function: stack -> PC */\n", $$0) } \
			else if (/\tret\t/) { printf ("%-50s\t/* store PC to link register, return from function: %s = PC; PC = %s */\n", $$0, $$3, $$4) } \
			else if (/\tjzx\t/) { printf ("%-50s\t/* jump if zero after shift and mask: if ((((%s << 16 | %s) >> %s) & (1 << (%s + 1) - 1)) == 0) PC = %s */\n", $$0, $$6, $$5, $$4, $$3, $$7) } \
			else if (/\tjnzx\t/) { printf ("%-50s\t/* jump if non-zero after shift and mask: if ((((%s << 16 | %s) >> %s) & (1 << (%s + 1) - 1)) != 0) PC = %s */\n", $$0, $$6, $$5, $$4, $$3, $$7) } \
			else if (/\tjext\t/) { printf ("%-50s\t/* jump if external condition true: if(%s) PC = %s */\n", $$0, $$3, $$4) } \
			else if (/\tjnext\t/) { printf ("%-50s\t/* jump if external condition true: if(!%s) PC = %s */\n", $$0, $$3, $$4) } \
			else if (/\tmov\t/) { printf ("%-50s\t/* copy data: %s = %s */\n", $$0, $$4, $$3) } \
			else if (/\ttkiph\t/) { printf ("%-50s\t/* TKIP S-Box lookup high: %s = SBOX[hi8(%s)] */\n", $$0, $$4, $$3) } \
			else if (/\ttkiphs\t/) { printf ("%-50s\t/* TKIP S-Box lookup high swapped: %s = byteswap(SBOX[hi8(%s)]) */\n", $$0, $$4, $$3) } \
			else if (/\ttkipl\t/) { printf ("%-50s\t/* TKIP S-Box lookup low: %s = SBOX[lo8(%s)] */\n", $$0, $$4, $$3) } \
			else if (/\ttkipls\t/) { printf ("%-50s\t/* TKIP S-Box lookup low swapped: %s = byteswap(SBOX[lo8(%s)]) */\n", $$0, $$4, $$3) } \
			else if (/\tsra\t/) { printf ("%-50s\t/* arithmetic right shift: %s = %s >> %s fillup sign */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tor\t/) { printf ("%-50s\t/* bitwise OR: %s = %s | %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tand\t/) { printf ("%-50s\t/* bitwise AND: %s = %s & %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\txor\t/) { printf ("%-50s\t/* bitwise XOR: %s = %s ^ %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsr\t/) { printf ("%-50s\t/* right shift: %s = %s >> %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsl\t/) { printf ("%-50s\t/* left shift: %s = %s << %s */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tsrx\t/) { printf ("%-50s\t/* shift right over two registers: %s = ((%s << 16) | %s >> %s) & (1 << (%s + 1) - 1) */\n", $$0, $$7, $$6, $$5, $$4, $$3) } \
			else if (/\trl\t/) { printf ("%-50s\t/* rotate left: %s = lrot(%s, %s bits) */\n", $$0, $$5, $$3, $$4) } \
			else if (/\trr\t/) { printf ("%-50s\t/* rotate right: %s = rrot(%s, %s bits) */\n", $$0, $$5, $$3, $$4) } \
			else if (/\tnand\t/) { printf ("%-50s\t/* clear bits (notmask + and): %s = %s & (~%s) */\n", $$0, $$5, $$3, $$4) } \
			else if (/\torx\t/) { printf ("%-50s\t/* or with shift and select: mask = ((1 << (%s + 1) - 1) << %s) | ((1 << (%s + 1) - 1) >> (16 - %s)); tmp = (%s << %s) | (%s >> (16 - %s)); %s  = (tmp & mask) | (%s & ~mask) */\n", $$0, $$3, $$4, $$3, $$4, $$5, $$4, $$5, $$4, $$7, $$6) } \
			else if (/\tnap/) { printf ("%-50s\t/* sleep until event */\n", $$0) } \
			else { print $0 } \
		}' $< > $@

ucode-patched.bin: ucode-patched.asm
	$(B43ASM) $< $@ --format raw-le32

clean:
	rm -f ucode.bin ucode-patched.bin ucode.asm ucode-commented.asm ucode-beautified.asm