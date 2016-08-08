#!/usr/bin/env python2

import os
import sys
import time
sys.path.append('../../../buildtools/binary_patcher')
sys.path.append('../../../buildtools/elffile')
import binary_patcher
from binary_patcher import *
import elffile

ef = elffile.open(name="patch.elf")

# This function gives us the address of a function in our patch.elf file It helps to identify where functions where automatically placed by the linker
def getSectionAddr(name):
	try:
		return next((header for header in ef.sectionHeaders if header.name == name), None).addr
	except:
		return 0

patch_firmware("../../../bootimg_src/firmware/fw_bcmdhd.orig.bin", 
    "fw_bcmdhd.bin", [
    # The text section is always required and contains code that is called by patches and hooks but not directly placed to predefined memory locations
    # in this example, this section contains the hello_world function.
    #
    # Linker output:
	# .text           0x0000000000180030       0x1c
	#  patch.o(.text .text.* .data .data.* .bss .bss.* .rodata .rodata.*)
	#  .text.hello_world
	#                 0x0000000000180030        0xc patch.o
	#                 0x0000000000180030                hello_world
	#  .rodata.str1.4
	#                 0x000000000018003c       0x10 patch.o
	#
	ExternalArmPatch(getSectionAddr(".text"), "text.bin"),

	# This function contains the hooking code for the wlc_ucode_download function
	#
	# Linker output:
	# .text.wlc_ucode_download_hook
	#                 0x0000000000180020       0x10
	#  patch.o(.text.wlc_ucode_download_hook)
	#  .text.wlc_ucode_download_hook
	#                 0x0000000000180020       0x10 patch.o
	#                 0x0000000000180020                wlc_ucode_download_hook
	#
	ExternalArmPatch(getSectionAddr(".text.wlc_ucode_download_hook"), "wlc_ucode_download_hook.bin"),

	# We replace the only branch-link instruction to the wlc_ucode_download function with a branch-link instruction to our hook
	BLPatch(0x1FD7F2, getSectionAddr(".text.wlc_ucode_download_hook")),

	# This line replaces the firmware version string that is printed to the console on startup to identify which firmware is loaded by the driver
	StringPatch(0x1FD31B, (os.getcwd().split('/')[-1] + " (" + time.strftime("%d.%m.%Y %H:%M:%S") + ")\n")[:52]), # 53 character string
	])
