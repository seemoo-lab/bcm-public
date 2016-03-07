#!/usr/bin/env python2

import sys
sys.path.append('../../buildtools/binary_patcher')
sys.path.append('../../buildtools/elffile')

import binary_patcher
from binary_patcher import *
import elffile

ef = elffile.open(name="wlc_bmac_recv.elf")

def getSectionAddr(name):
	return next((header for header in ef.sectionHeaders if header.name == name), None).addr

FW_FILE = "../../bootimg_src/firmware/fw_bcmdhd.orig.bin"

patchfile0 = 'filter.bin'
patchfile1 = 'wlc_bmac_recv.bin'

# wlc_bmac_recv()
detour0 = ExternalArmPatch(0x180050, patchfile0)
detour1 = ExternalArmPatch(0x1AAD98, patchfile1)
detour15 = ExternalArmPatch(getSectionAddr(".text"), "text.bin")

#2nd call of wlc_bmac_mctrl() in wlc_coreinit()
# mask
detour2 = GenericPatch4(0x1AB82C, 0x41d60000)

# val
detour3 = GenericPatch4(0x1AB828, 0x41d20000)

#replace revision with 'SEEMOO!'
detour4 = GenericPatch4(0x1d3a48, 0x4D454553)
detour5 = GenericPatch4(0x1d3a4c, 0x29214F4F)

#Android 6
patch_firmware(FW_FILE,
    "fw_bcmdhd.bin", [
        detour0,
        detour1,
        detour15,
        detour2,
        detour3,
        detour4,
        detour5])
