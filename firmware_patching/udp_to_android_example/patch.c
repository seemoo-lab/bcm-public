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
#include "../include/bcmdhd/bcmsdpcm.h"
#include "../include/bcmdhd/bcmcdc.h"

unsigned char bdc_ethernet_ipv6_udp_header_array[] = {
  0x20, 0x00, 0x00, 0x00,				/* BDC Header */
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,	/* ETHERNET: Destination MAC Address */
  'N', 'E', 'X', 'M', 'O', 'N',			/* ETHERNET: Source MAC Address */
  0x86, 0xDD,							/* ETHERNET: Type */
  0x60, 0x00, 0x00, 0x00,				/* IPv6: Version / Traffic Class / Flow Label */
  0x00, 0x08,							/* IPv6: Payload Length */
  0x88,									/* IPv6: Next Header = UDPLite */
  0x01,									/* IPv6: Hop Limit */
  0xFF, 0x02, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x01,				/* IPv6: Source IP */
  0xFF, 0x02, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,				/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x01,				/* IPv6: Destination IP */
  0xD6, 0xD8,							/* UDPLITE: Source Port = 55000 */
  0xD6, 0xD8,							/* UDPLITE: Destination Port = 55000 */
  0x00, 0x08,							/* UDPLITE: Checksum Coverage */
  0x52, 0x46,							/* UDPLITE: Checksum only over UDPLITE header*/
};

void
send_sdio_frame(void) {
	struct sk_buff *p = 0;
	struct osl_info *osh = OSL_INFO_ADDR;
	struct bdc_ethernet_ipv6_udp_header *hdr;

	char text[] = "Hello Android!";

	// Create a new sk_buff to hold the headers and the payload
	p = pkt_buf_get_skb(osh, sizeof(bdc_ethernet_ipv6_udp_header_array) + sizeof(text));

	// Copy the headers to the sk_buff
	memcpy(p->data, bdc_ethernet_ipv6_udp_header_array, sizeof(bdc_ethernet_ipv6_udp_header_array));

	hdr = p->data;

	// Set the IPv6 payload length
	hdr->ipv6.payload_length = htons(sizeof(struct udp_header) + sizeof(text));
	
	// Copy the text to the payload of the frame
	memcpy(hdr->payload, text, sizeof(text));

	// Send the frame over SDIO to the Linux driver, where it will be injected into the network stack
	dngl_sendpkt(SDIO_INFO_ADDR, p, SDPCM_DATA_CHANNEL);

	printf("sdio frame transmitted\n");
}

/**
 *	is called before the wlc_ioctl function to send our hello Android frame
 */
__attribute__((naked)) void
wlc_ioctl_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"				// Push the registers that could be modified by a call to a C function
		"bl send_sdio_frame\n"			// Call a C function
		"pop {r0-r3,lr}\n"				// Pop the registers that were saved before
		"push {r4-r11,lr}\n"
		"b wlc_ioctl+4\n"					// Call the hooked function
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