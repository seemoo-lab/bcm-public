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

	ExternalArmPatch(getSectionAddr(".text.wlc_channel_set_chanspec_hook"), "wlc_channel_set_chanspec_hook.bin"),
	BPatch(0x1AE2BC, getSectionAddr(".text.wlc_channel_set_chanspec_hook")),

	ExternalArmPatch(getSectionAddr(".text.wlc_iovar_change_handler_hook"), "wlc_iovar_change_handler_hook.bin"),
	BPatch(0x19B25C, getSectionAddr(".text.wlc_iovar_change_handler_hook")),

	ExternalArmPatch(getSectionAddr(".text.wlc_set_chanspec_hook"), "wlc_set_chanspec_hook.bin"),
	BLPatch(0x19D51C, getSectionAddr(".text.wlc_set_chanspec_hook")),

	ExternalArmPatch(getSectionAddr(".text.wlc_set_var_chanspec_hook"), "wlc_set_var_chanspec_hook.bin"),
	GenericPatch4(0x19B6F8, getSectionAddr(".text.wlc_set_var_chanspec_hook")+1),

	ExternalArmPatch(getSectionAddr(".text.wlc_valid_chanspec_ext_hook"), "wlc_valid_chanspec_ext_hook.bin"),
	BPatch(0x1ADA64, getSectionAddr(".text.wlc_valid_chanspec_ext_hook")),

	ExternalArmPatch(getSectionAddr(".text.phy_read_reg_hook"), "phy_read_reg_hook.bin"),
	BPatch(0x1C3D32, getSectionAddr(".text.phy_read_reg_hook")),

	ExternalArmPatch(getSectionAddr(".text.phy_write_reg_hook"), "phy_write_reg_hook.bin"),
	BPatch(0x1c3d48, getSectionAddr(".text.phy_write_reg_hook")),

	ExternalArmPatch(getSectionAddr(".text.phy_reg_and_hook"), "phy_reg_and_hook.bin"),
	BPatch(0x1c465c, getSectionAddr(".text.phy_reg_and_hook")),

	ExternalArmPatch(getSectionAddr(".text.phy_reg_or_hook"), "phy_reg_or_hook.bin"),
	BPatch(0x1c4676, getSectionAddr(".text.phy_reg_or_hook")),

	ExternalArmPatch(getSectionAddr(".text.write_radio_reg_hook"), "write_radio_reg_hook.bin"),
	BPatch(0x1c3cfe, getSectionAddr(".text.write_radio_reg_hook")),

	#GenericPatch4(0x87FD2, 0x096C0001),

	# This line replaces the firmware version string that is printed to the console on startup to identify which firmware is loaded by the driver
	StringPatch(0x1FD31B, (os.getcwd().split('/')[-1] + " (" + time.strftime("%d.%m.%Y %H:%M:%S") + ")\n")[:52]), # 53 character string
	])
