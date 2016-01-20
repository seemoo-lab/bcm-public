#!/usr/bin/env python2

import sys
sys.path.append('../../buildtools/binary_patcher')
import binary_patcher
from binary_patcher import *

FW_FILE = "../../bootimg_src/firmware/fw_bcmdhd.orig.bin"

patchfile1 = 'wlc_bmac_recv.bin'

# wlc_bmac_recv()
detour1 = ExernalArmPatch(0x1AAD98, patchfile1)

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
        detour1,
        detour2,
        detour3,
        detour4,
        detour5])
