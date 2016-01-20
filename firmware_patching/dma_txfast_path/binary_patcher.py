#!/usr/bin/env python2

"""
Based on the 'generic_patcher' by shoote:
https://github.com/shoote/generic_patcher
"""

from subprocess import check_call, STDOUT 
from os import unlink
import os
from os.path import exists
import struct
import binascii
from bitstring import ConstBitStream

RAM_START = 0x180000

class BasePatch(object):
    def __init__(self, pos, data):
        self.pos = pos
        self.data = data
    
    def apply(self, firmware, **kargs):
        fw_pos = self.pos - RAM_START
        firmware += max(0, fw_pos - len(firmware)) * "\x00"
        return firmware[:fw_pos] + self.data + firmware[fw_pos+len(self.data):]

class ExernalArmPatch(BasePatch):
    def __init__(self, pos, infile, extra_data=""):
        self.pos = pos
        self.infile = infile
        self.extra_data = extra_data

    def _delete_temp(self,path):
        if exists(path):
            unlink(path)

    def _generate_bytes(self):
        try:
            data = file(self.infile,'rb').read() + self.extra_data
            print "data_len: %d" % len(data)
            return data
        finally:
            print "success read"

    def get_bytes(self):
        return self._generate_bytes()
    
    def apply(self, firmware, **kargs):
        self.data = self._generate_bytes()
        return super(ExernalArmPatch, self).apply(firmware)


class BPatch(BasePatch):
    """
    Generates a B.W opcode patch
    """
    def __init__(self, pos, to, real_from=None):
        """
        Creates a B.W patch at pos which will jump to the 'to' destination.
        The read_from paramter needs to be used when the opcodes are copied
        from the pos address to another, and it should contain the address
        which the patch will run from.
        Props: https://github.com/DiGMi/generic_patcher/commit/366ff9f60f9e3df070cfcb84a0be01e13d6cbe19#diff-e8040c3645a0f37b6c027dd5bb240603
        """
        if real_from == None:
            real_from = pos
        self.pos = pos
        self.data = self.patch_b(real_from, to)

    def patch_b(self, src, dst):
        diff = dst - src - 4
        diff_upper = (diff >> 12) & 0x3ff
        diff_lower = ((diff & 0xfff) >> 1) | (((diff >>22)&1) << 11) | (((diff >> 23) & 1) <<13)
        a = (0xF000 | diff_upper)# << 16
        b = (0x9000 | diff_lower)
        if diff < 0:
            a |= 0x0400
        else:
            b = b^ 0x2800
        c = (a << 16) | b
        print "patched: %X to call %X B.W opcode: %s" % (src, dst,
                struct.pack("<HH", a, b).encode('hex'))
        return struct.pack("<HH", a, b)  

        
class BLPatch(BasePatch):
    def __init__(self, pos, to):
        self.pos = pos
        self.to = to

    def patch_bl(self, src, dst):

        if(src < RAM_START):
            print "ERROR, trying to patch ROM"
            return
        
        diff = dst - src - 4
        
        #looks like we jump to a lower addr (not necessary)
        if(diff < 0):
            diff &= 0xffffffff

        diff_upper = (diff >> 12) & 0x7ff
        diff_lower = (diff & 0xfff) >> 1
        a = (0xF000 | diff_upper)# << 16
        b = (0xF800 | diff_lower)
        c = (a << 16) | b
        print "patched: %X to call %X BL opcode: %X" % (src, dst, c)
        packed_data = struct.pack("<HH", a, b)
        print "packed: ", binascii.hexlify(packed_data)
        return packed_data

    def apply(self, firmware, **kargs):
        self.data = str(self.patch_bl(self.pos, self.to))
        return super(BLPatch, self).apply(firmware)

class GenericPatch4(BasePatch):
    """
    Just replace a 4 Byte word
    """
    def __init__(self, pos, to):
        self.pos = pos
        self.to = to

    def patch_gen(self, subst):
        packed_data = struct.pack("<I", subst)
        print "packed: ", binascii.hexlify(packed_data)
        return packed_data

    def apply(self, firmware, **kargs):
        self.data = str(self.patch_gen(self.to))
        return super(GenericPatch4, self).apply(firmware)

class GenericPatch2(BasePatch):
    """
    Just replace a 2 Byte word
    """
    def __init__(self, pos, to):
        self.pos = pos
        self.to = to

    def patch_gen(self, subst):
        packed_data = struct.pack("<H", subst)
        print "packed: ", binascii.hexlify(packed_data)
        return packed_data

    def apply(self, firmware, **kargs):
        self.data = str(self.patch_gen(self.to))
        return super(GenericPatch2, self).apply(firmware)

        
def patch_firmware(src, dst, patchs, extra = "", **kargs):
    """
    Patch the firmware blob

    :param src: original file (input)
    :param dest: patched file (output)
    """
    firmware = file(src,'rb').read()
    for p in patchs:
        firmware = p.apply(firmware, **kargs)
    firmware += extra
    firmware = file(dst,'wb').write(firmware)


patchfile1 = 'dngl_sendpkt.bin'
patchfile2 = 'dngl_sendpkt_2.bin'
patchfile3 = 'dngl_sendpkt_3.bin'

#Android 6
patch_firmware("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", 
    "fw_bcmdhd.bin", [
        ExernalArmPatch(0x180050, "dngl_sendpkt.bin"),
	BLPatch(0x182796 - 0x2400, 0x184F14),
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
	BLPatch(0x1824C6 - 0x2400, 0x1844B2),
	BPatch( 0x182CB0, 0x180350), # call to dngl_sendpkt
	BLPatch(0x182920, 0x180350), # call to dngl_sendpkt
    ])

