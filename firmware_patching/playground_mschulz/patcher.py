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
	ExternalArmPatch(getSectionAddr(".text"), "text.bin"),

#	ExternalArmPatch(getSectionAddr(".text.before_before_initialize_memory_hook"), "before_before_initialize_memory_hook.bin"),
#	GenericPatch4(0x181240, getSectionAddr(".text.before_before_initialize_memory_hook")+1),

#	ExternalArmPatch(getSectionAddr(".text.tr_pref_abort_hook"), "tr_pref_abort_hook.bin"), # patch to stay in abort mode to handle exception, original codes switches to system mode

#	ExternalArmPatch(getSectionAddr(".text.tr_data_abort_hook"), "tr_data_abort_hook.bin"), # patch to stay in abort mode to handle exception, original codes switches to system mode

#	ExternalArmPatch(getSectionAddr(".text.handle_exceptions"), "handle_exceptions.bin"), # patch to stay in abort mode to handle exception, original codes switches to system mode

	#GenericPatch4(0x18534E, 0xBF00BF00), # write nops over the si_update_chipcontrol_shm(sii, 5, 1, 0) call in si_setup_cores
	#GenericPatch4(0x18535E, 0xBF00BF00), # write nops over the si_update_chipcontrol_shm(sii, 5, 8, 0) call in si_setup_cores
#	The following patch, that is necessary to access debug registers in an interrupt handler breaks normal wifi operation
#	GenericPatch4(0x18536A, 0xBF00BF00), # write nops over the si_update_chipcontrol_shm(sii, 5, 16, 0) call in si_setup_cores

#	ExternalArmPatch(getSectionAddr(".text.nex_ioctl_handler"), "nex_ioctl_handler.bin"),
#	GenericPatch4(0x195658, getSectionAddr(".text.nex_ioctl_handler")+1),
#	GenericPatch4(0x19565C, getSectionAddr(".text.nex_ioctl_handler")+1),
#	GenericPatch4(0x195660, getSectionAddr(".text.nex_ioctl_handler")+1),
#	GenericPatch4(0x195664, getSectionAddr(".text.nex_ioctl_handler")+1),

	ExternalArmPatch(getSectionAddr(".text.wlc_ucode_download_hook"), "wlc_ucode_download_hook.bin"),
	BLPatch(0x1fd7f2, getSectionAddr(".text.wlc_ucode_download_hook")),
	ExternalArmPatch(0x1fd820, "../d11/ucode-patched.bin"),

	StringPatch(0x1FD31B, "build: " + time.strftime("%d.%m.%Y %H:%M:%S") + "\n"), # 53 character string
	])
