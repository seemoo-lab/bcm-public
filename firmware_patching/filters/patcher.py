#!/usr/bin/env python2

import sys
sys.path.append('../../buildtools/binary_patcher')
import binary_patcher
from binary_patcher import *

FW_FILE = "../../bootimg_src/firmware/fw_bcmdhd.orig.bin"

#patchfile1 = 'filter_handler.bin'
#v2
patchfile1 = 'filter_handler.bin'

# filter_handler()
detour1 = ExernalArmPatch(0x180050, patchfile1)

# Substitue B.W to pkt_buf_free_skb() with our own code
#detour2 = BPatch(0x182AC8, 0x180050);
#v2
detour2 = BLPatch(0x182C60, 0x180050);
detour3 = GenericPatch4(0x180BB4, 0x180051);

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
