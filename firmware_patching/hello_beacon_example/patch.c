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

#define BCMEXTRAHDROOM 202

char pkt[] = {
	0x80, 0x00, 0x00, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 			/* destination address */
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,				/* source address */
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 			/* BSS address */
	0x10, 0x00, 									/* sequence and fragment numbers */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/* timestamp */
	0x64, 0x00, 0x21, 0x05, 
	0x00, 											/* tag number: SSID parameter set */
	0x06, 											/* tag length */
	'N', 'E', 'X', 'M', 'O', 'N',		 			/* SSID */
};

/**
 *	add data to the start of a buffer
 */
void *
skb_push(sk_buff *p, unsigned int len)
{
	p->data -= len;
	p->len += len;

	return p->data;
}

/**
 *	remove data from the start of a buffer
 */
void *
skb_pull(sk_buff *p, unsigned int len)
{
	p->data += len;
	p->len -= len;

	return p->data;
}

void
wlc_radio_upd_hook_in_c(void)
{
	sk_buff *p;
	struct wlc_info *wlc = WLC_INFO_ADDR;
	void *bsscfg = wlc_bsscfg_find_by_wlcif(wlc, 0);
	void *scb;

	// create a new sk_buff to hold the beacon frame and additional headers
	p = pkt_buf_get_skb(wlc->osh, sizeof(pkt) + 202);

	// shift the beginning of the frame to leave space for additional headers
	skb_pull(p, 202);

	// copy the example frame to the buffer
	memcpy(p->data, pkt, sizeof(pkt));

	// get station control block (scb) for given mac address
	scb = __wlc_scb_lookup(wlc, bsscfg, pkt, 0);

	// set the scb's bsscfg entry
	wlc_scb_set_bsscfg(scb, bsscfg);

	// send the frame with the lowest possible rate
	wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, 0, 0);
}

__attribute__((naked)) void
wlc_radio_upd_hook(void)
{
	asm(
		"push {lr}\n"
		"bl wlc_radio_upd\n"
		"push {r0-r3}\n"
		"bl wlc_radio_upd_hook_in_c\n"
		"pop {r0-r3}\n"
		"pop {pc}\n"
		);
}

/**
 *	Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_180800(void)
{
	;
}