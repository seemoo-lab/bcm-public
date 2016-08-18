#!/usr/bin/env python2

import sys
sys.path.append('../../../buildtools/binary_patcher')
import binary_patcher
from binary_patcher import *

FW_FILE = "../../../bootimg_src/firmware/fw_bcmdhd.orig.bin"

patchfile1 = 'user2fw.bin'

detour1 = ExternalArmPatch(0x180050, patchfile1)

detour2 = BLPatch(0x182C60, 0x180050);
detour3 = GenericPatch4(0x180BB4, 0x180050);

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
