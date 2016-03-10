#!/usr/bin/env python2

import sys
import time
sys.path.append('../../buildtools/binary_patcher')
sys.path.append('../../buildtools/elffile')
import binary_patcher
from binary_patcher import *
import elffile

ef = elffile.open(name="patch.elf")

def getSectionAddr(name):
	return next((header for header in ef.sectionHeaders if header.name == name), None).addr

patch_firmware("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", 
    "fw_bcmdhd.bin", [
	ExernalArmPatch(0x180050, "dngl_sendpkt.bin"),
	BLPatch(0x182796 - 0x2400, 0x184F14), # START some functions called by dngl_sendpkt or one of its subfunctions
	BLPatch(0x1827A4 - 0x2400, 0x184F64),
	BLPatch(0x1827CE - 0x2400, 0x184F64),
	BLPatch(0x182586 - 0x2400, 0x184F64),
	BLPatch(0x1827C0 - 0x2400, 0x181418),
	BLPatch(0x182836 - 0x2400, 0x181418),
	BLPatch(0x182854 - 0x2400, 0x181418),
	BLPatch(0x182868 - 0x2400, 0x181418),
	BLPatch(0x182888 - 0x2400, 0x181418),
	BLPatch(0x1827E6 - 0x2400, 0x01269C),
	BLPatch(0x1827FE - 0x2400, 0x01360C),
	BLPatch(0x182808 - 0x2400, 0x1820BC),
	BLPatch(0x182818 - 0x2400, 0x013090),
	BLPatch(0x1825AA - 0x2400, 0x0134A8),
	BLPatch(0x182690 - 0x2400, 0x0134A8),
	BLPatch(0x182492 - 0x2400, 0x0134E4),
	BLPatch(0x18250A - 0x2400, 0x0133FC),
	BLPatch(0x182678 - 0x2400, 0x013430),
	BLPatch(0x1825C4 - 0x2400, 0x0133D8),
	BLPatch(0x1825B2 - 0x2400, 0x0133D8),
	BLPatch(0x18245E - 0x2400, 0x0133D8),
	BLPatch(0x182534 - 0x2400, 0x0133D8),
	BLPatch(0x18254E - 0x2400, 0x0133D8),
	BLPatch(0x18247E - 0x2400, 0x0135B8),
	BLPatch(0x1825F2 - 0x2400, 0x182148),
	BLPatch(0x182680 - 0x2400, 0x182148),
	BLPatch(0x1826CE - 0x2400, 0x182148),
	BLPatch(0x182698 - 0x2400, 0x182148),
	BLPatch(0x182474 - 0x2400, 0x1820C0),
	BLPatch(0x182610 - 0x2400, 0x016550),
	BLPatch(0x182710 - 0x2400, 0x016550),
	BLPatch(0x18272E - 0x2400, 0x016550),
	BLPatch(0x1826AC - 0x2400, 0x016608),
	BLPatch(0x1826E2 - 0x2400, 0x016608),
	BLPatch(0x182702 - 0x2400, 0x016608),
	BLPatch(0x18252C - 0x2400, 0x01B874),
	BPatch( 0x182546 - 0x2400, 0x014C6C),
	BPatch( 0x182560 - 0x2400, 0x014C54),
	BLPatch(0x1824C6 - 0x2400, 0x1844B2), # END some functions called by dngl_sendpkt or one of its subfunctions
#	BPatch( 0x182CB0, 0x180350), # call to dngl_sendpkt
#	BLPatch(0x182920, 0x180350), # call to dngl_sendpkt
	ExernalArmPatch(getSectionAddr(".text"), "text.bin"),
#	ExernalArmPatch(getSectionAddr(".text.dma_txfast_hook"), "dma_txfast_hook.bin"),
#	GenericPatch4(0x180C34, getSectionAddr(".text.dma_txfast_hook")+1), # function pointer table entry to dma_txfast replaced by dma_txfast_hook
#	GenericPatch4(0x1D53C4, getSectionAddr(".text.dma_txfast_hook")+1), # function pointer table entry to dma_txfast replaced by dma_txfast_hook
#	ExernalArmPatch(getSectionAddr(".text.wlc_txfifo_hook"), "wlc_txfifo_hook.bin"),
#	BPatch(0x193744, getSectionAddr(".text.wlc_txfifo_hook")), # replace the push instrunction of the original wlc_txfifo function by a branch to our hook
	ExernalArmPatch(getSectionAddr(".text.interrupt_enable_hook"), "interrupt_enable_hook.bin"),
	BPatch(0x1838ac, getSectionAddr(".text.interrupt_enable_hook")),
	ExernalArmPatch(getSectionAddr(".text.wlc_ucode_download_hook"), "wlc_ucode_download_hook.bin"),
	BLPatch(0x1FD7F2, getSectionAddr(".text.wlc_ucode_download_hook")),
	ExernalArmPatch(getSectionAddr(".text.before_before_initialize_memory_hook"), "before_before_initialize_memory_hook.bin"),
	GenericPatch4(0x181240, getSectionAddr(".text.before_before_initialize_memory_hook")+1),
	ExernalArmPatch(getSectionAddr(".text.interrupt_handler"), "interrupt_handler.bin"),
#	GenericPatch4(0x1ECA84, getSectionAddr(".text.interrupt_handler")+1),
#	ExernalArmPatch(getSectionAddr(".text.setup_some_stuff_hook"), "setup_some_stuff_hook.bin"),
#	BLPatch(0x1F2358, getSectionAddr(".text.setup_some_stuff_hook")),
#	ExernalArmPatch(getSectionAddr(".text.bus_binddev_rom_hook"), "bus_binddev_rom_hook.bin"),
#	GenericPatch4(0x1D9B08, getSectionAddr(".text.bus_binddev_rom_hook")+1), # function pointer in the dngl_pointer_table
#	ExernalArmPatch(getSectionAddr(".text.sub_1ECAB0_hook"), "sub_1ECAB0_hook.bin"),
#	BLPatch(0x18389A, getSectionAddr(".text.sub_1ECAB0_hook")),
#	ExernalArmPatch(getSectionAddr(".text.function_with_huge_jump_table_hook"), "function_with_huge_jump_table_hook.bin"),
#	BPatch(0x19b25c, getSectionAddr(".text.function_with_huge_jump_table_hook")),
#	ExernalArmPatch(getSectionAddr(".text.handle_ioctl_cmd_hook"), "handle_ioctl_cmd_hook.bin"),
#	BPatch(0x18AE72, getSectionAddr(".text.handle_ioctl_cmd_hook")),
#	ExernalArmPatch(getSectionAddr(".text.console_size_r0"), "console_size_r0.bin"), # change console size from 0x800 to 0x2000
#	ExernalArmPatch(getSectionAddr(".text.console_size_r2"), "console_size_r2.bin"), # change console size from 0x800 to 0x2000
#	GenericPatch4(0x1ED722, 0x00000000), # NOP the call to initialize_device_core to not initialize the D11 core
#	GenericPatch4(0x1ED72A, 0x00000000), # NOP the check if D11 core initialization returned an error
#	GenericPatch2(0x19B260, int('1011111000000000',2)), # replace function with huge jump table with a breakpoint
#	GenericPatch4(0x19B348, 0xFFFFFFFF), # breakpoint in function with huge jump table before jumping
#	GenericPatch4(0x19b25c, 0xFFFFFFFF), # breakpoint in function with huge jump table before jumping
	StringPatch(0x1FD31B, "build: " + time.strftime("%d.%m.%Y %H:%M:%S") + "\n"), # 53 character string
	])
