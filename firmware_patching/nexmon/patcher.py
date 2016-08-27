#!/usr/bin/env python2

import os
import sys
import time
sys.path.append('../../buildtools/binary_patcher')
sys.path.append('../../buildtools/elffile')
import binary_patcher
from binary_patcher import *
import elffile

ef = elffile.open(name="patch.elf")

# This function gives us the address of a function in our patch.elf file It helps to identify where functions where automatically placed by the linker
def getSectionAddr(name):
	return next((header for header in ef.sectionHeaders if header.name == name), None).addr

patch_firmware("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", 
    "fw_bcmdhd.bin", [
	# The text section is always required and contains code that is called by patches and hooks but not directly placed to predefined memory locations
	ExternalArmPatch(getSectionAddr(".text"), "text.bin"),

	ExternalArmPatch(getSectionAddr(".text.ucode_compression_code"), "ucode_compression_code.bin"),
	ExternalArmPatch(getSectionAddr(".text.wlc_ucode_write_compressed"), "wlc_ucode_write_compressed.bin"),
	BLPatch(0x1f4f08, getSectionAddr(".text.wlc_ucode_write_compressed")),

	# ExternalArmPatch instructions copy the contents of a binary file to an address in the firmware
	# ExternalArmPatch(<address to store binary blob>, <file name>),

	# BLPatch instructions create branch-link instructions at a specified location to jump to a target address and set the link register to the following instruction
	# BLPatch(<address for BL instruction>, <target address>),

	# BPatch instructions create branch-link instructions at a specified location to jump to a target address without modifying the link register
	# BPatch(<address for BL instruction>, <target address>),

	# StringPatch instructions copy a string to a specified address in the firmware
	# StringPatch(<address to store text>, <text>),

	# GenericPatch4 instructions replace a dword (four bytes) at a given address in the firmware
	# This is helpful to patch target addresses in jump tables. Keep in mind to use the target function address +1 to jump to a Thumb function
	# GenericPatch4(<address to replace dword>, <four byte value>),

	# GenericPatch2 instructions replace a word (two bytes) at a given address in the firmware
	# 0xBF00 can be used to place a NOP instruction
	# GenericPatch2(<address to replace word>, <two byte value>),

	# Hook the call to wl_monitor in the wlc_monitor function
	ExternalArmPatch(getSectionAddr(".text.wl_monitor_hook"), "wl_monitor_hook.bin"),
	BLPatch(0x18DA30, getSectionAddr(".text.wl_monitor_hook")),

	# Add the dma_attach_hook function to the firmware
	ExternalArmPatch(getSectionAddr(".text.dma_attach_hook"), "dma_attach_hook.bin"),

	# Hook the first call to wlc_bmac_attach_dmapio to increase the rx extra header size
	BLPatch(0x1F4FCE, getSectionAddr(".text.dma_attach_hook")),

	ExternalArmPatch(getSectionAddr(".text.handle_sdio_xmit_request_hook"), "handle_sdio_xmit_request_hook.bin"),
	BPatch(0x182AAA, getSectionAddr(".text.handle_sdio_xmit_request_hook")),
	GenericPatch4(0x180BCC, getSectionAddr(".text.handle_sdio_xmit_request_hook") + 1),

	# This line replaces the firmware version string that is printed to the console on startup to identify which firmware is loaded by the driver
	StringPatch(0x1FD31B, (os.getcwd().split('/')[-1] + " (" + time.strftime("%d.%m.%Y %H:%M:%S") + ")\n")[:52]), # 53 character string
	])
