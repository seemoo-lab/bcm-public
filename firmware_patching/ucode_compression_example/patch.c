/***************************************************************************
 *                                                                         *
 *          ###########   ###########   ##########    ##########           *
 *         ############  ############  ############  ############          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ##            ##            ##   ##   ##  ##        ##          *
 *         ###########   ####  ######  ##   ##   ##  ##    ######          *
 *          ###########  ####  #       ##   ##   ##  ##    #    #          *
 *                   ##  ##    ######  ##   ##   ##  ##    #    #          *
 *                   ##  ##    #       ##   ##   ##  ##    #    #          *
 *         ############  ##### ######  ##   ##   ##  ##### ######          *
 *         ###########    ###########  ##   ##   ##   ##########           *
 *                                                                         *
 *            S E C U R E   M O B I L E   N E T W O R K I N G              *
 *                                                                         *
 * Warning:                                                                *
 *                                                                         *
 * Our software may damage your hardware and may void your hardware’s      *
 * warranty! You use our tools at your own risk and responsibility!        *
 *                                                                         *
 * License:                                                                *
 * Copyright (c) 2015 NexMon Team                                          *
 *                                                                         *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the         *
 * "Software"), to deal in the Software without restriction, including     *
 * without limitation the rights to use, copy, modify, merge, publish,     *
 * distribute copies of the Software, and to permit persons to whom the    *
 * Software is furnished to do so, subject to the following conditions:    *
 *                                                                         *
 * The above copyright notice and this permission notice shall be included *
 * in all copies or substantial portions of the Software.                  *
 *                                                                         *
 * Any use of the Software which results in an academic publication or     *
 * other publication which includes a bibliography must include a citation *
 * to the author's publication "M. Schulz, D. Wegemer and M. Hollick.      *
 * NexMon: A Cookbook for Firmware Modifications on Smartphones to Enable  *
 * Monitor Mode.".                                                         *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,    *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE       *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                  *
 *                                                                         *                                                       *
 **************************************************************************/

#include "../include/bcm4339.h"	// contains addresses specific for BCM4339
#include "../include/debug.h"	// contains macros to access the debug hardware
#include "../include/wrapper.h"	// wrapper definitions for functions that already exist in the firmware
#include "../include/structs.h"	// structures that are used by the code in the firmware
#include "../include/helper.h"	// useful helper functions

/**
 *  Contains tinflate_partial and other functions needed for deflate decompression
 */
#include "tinflate.c"

/**
 *	Contains ucode_compressed_bin and ucode_compressed_bin_len
 */
#include "ucode-compressed.c"

void
wlc_bmac_read_objmem32(struct wlc_hw_info *wlc_hw, unsigned int offset, unsigned int *val, int sel)
{
	volatile struct d11regs *regs;

	regs = wlc_hw->regs;
	regs->objaddr = sel | ((offset & 0xfffffffc) >> 2);
	regs->objaddr;

	*val = regs->objdata;
}

void
wlc_bmac_read_objmem64(struct wlc_hw_info *wlc_hw, unsigned int offset, unsigned int *val_low, unsigned int *val_high, int sel)
{
	volatile struct d11regs *regs;

	regs = wlc_hw->regs;
	regs->objaddr = sel | ((offset & 0xfffffff8) >> 2);
	regs->objaddr;

	*val_low = regs->objdata;

	regs->objaddr = (sel | ((offset & 0xfffffff8) >> 2)) + 1;
	regs->objaddr;

	*val_high = regs->objdata;
}

void
wlc_bmac_write_objmem64(struct wlc_hw_info *wlc_hw, unsigned int offset, unsigned int val_low, unsigned int val_high, int sel)
{
	volatile struct d11regs *regs;

	regs = wlc_hw->regs;
	regs->objaddr = sel | ((offset & 0xfffffff8) >> 2);
	regs->objaddr;

	regs->objdata = val_low;

	regs->objaddr = (sel | ((offset & 0xfffffff8) >> 2)) + 1;
	regs->objaddr;

	regs->objdata = val_high;
}

void
wlc_bmac_write_objmem_byte(struct wlc_hw_info *wlc_hw, unsigned int offset, unsigned char value, int sel)
{
	unsigned int low;
	unsigned int high;

    // first we read in one QWORD of existing bytes stored in the d11 object memory
	wlc_bmac_read_objmem64(wlc_hw, offset, &low, &high, sel);
    
    // then we replace the byte that should be written in this QWORD
	if (offset & 4) {
		((unsigned char *) &high)[offset & 3] = value;
	} else {
		((unsigned char *) &low)[offset & 3] = value;
	}

    // then we write back the changed QWORD into the object memory. We always access
    // a whole QWORD to be able to read back the written value at the next call to the
    // wlc_bmac_read_objmem64 function. Writing less than 64 bits (one QWORD) does not
    // deliver the new but the old value on the next read.
	wlc_bmac_write_objmem64(wlc_hw, offset, low, high, sel);
}

unsigned char
wlc_bmac_read_objmem_byte(struct wlc_hw_info *wlc_hw, unsigned int offset, int sel)
{
	unsigned int val;

	wlc_bmac_read_objmem32(wlc_hw, offset, &val, sel);
    
    return ((unsigned char *) &val)[offset & 0x3];
}

/**
 *  Function used by tinflate_partial to write a byte to an address in the output buffer
 *  here it is implemented to directly write to the object memory of the d11 core
 */
void
tinflate_write_objmem(void *out_base, unsigned long idx, unsigned char value)
{
    wlc_bmac_write_objmem_byte((struct wlc_hw_info *) out_base, idx, value, 0);
}

/**
 *  Function used by tinflate_partial to read a byte from an address in the output buffer
 *  here it is implemented to directly read from the object memory of the d11 core
 */
unsigned char
tinflate_read_objmem(void *out_base, unsigned long idx)
{
    return wlc_bmac_read_objmem_byte((struct wlc_hw_info *) out_base, idx, 0);
}

void
wlc_ucode_write_alternative(void *wlc_hw, const int ucode[], const unsigned int nbytes)
{
    int i = 0;
    char *ucode_c = (char *) ucode;

    for (i = 0; i < nbytes; i++) {
        wlc_bmac_write_objmem_byte(wlc_hw, i, ucode_c[i], 0);
    }

    /* state: Decompression state buffer to pass to tinflate_block(). */
    DecompressionState state;

    /**** Clear decompression state buffer. ****/
    state.state     = INITIAL;
    state.out_ofs   = 0;
    state.bit_accum = 0;
    state.num_bits  = 0;
    state.final     = 0;
    /* No other fields need to be cleared. */

    /**** Call tinflate_partial() to do the actual decompression. ****/
    tinflate_partial(ucode_compressed_bin, ucode_compressed_bin_len,
        wlc_hw, 100000, 0, &state, sizeof(state), tinflate_write_objmem, tinflate_read_objmem);
}

/**
 *	Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_180800(void)
{
	;
}
