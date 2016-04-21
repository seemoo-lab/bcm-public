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

#include "../include/bcm4339.h"		// contains addresses specific for BCM4339
#include "../include/debug.h"		// contains macros to access the debug hardware
#include "../include/wrapper.h"		// wrapper definitions for functions that already exist in the firmware
#include "../include/structs.h"		// structures that are used by the code in the firmware
#include "../include/helper.h"		// useful helper functions
#include "nexioctl.h"				// holds the ioctl numbers used by the driver

/**
 *	Handler for our own ioctls that are not used by the firmware.
 *	NEX_TEST_IOCTL_1 sends a string back to the driver
 *	NEX_TEST_IOCTL_2 prints a string to the WiFi chips console
 */
int
nex_ioctl_handler_in_c(struct wlc_info *wlc, int cmd, void *arg, unsigned int len)
{
	printf("%s: cmd=%d, arg=%08x, len=%d\n", __FUNCTION__, cmd, (int) arg, len);

	switch(cmd) {
		case NEX_TEST_IOCTL_1:
			if (len >= 17) {
				memcpy(arg, "hello driver!\0", 13);
			}
			break;
		case NEX_TEST_IOCTL_2:
			printf("NEX_TEST_IOCTL_2: %s\n", (char *) arg);
			break;
		case NEX_TEST_IOCTL_3:
		case NEX_TEST_IOCTL_4:
			break;
	}

	return 0;
}

/**
 *	This handler can be made an entry in the wlc_ioctl jump table (0x19561C for cmd == 1, 0x0x195AFC for cmd == 312)
 *	it is used to call the nex_ioctl_handler_in_c and then execute the code to return from wlc_ioctl
 */
__attribute__((naked)) void
nex_ioctl_handler(void)
{
	asm(
		"mov r0, r4\n"							// Store wlc in r0
		"mov r1, r8\n"							// Store cmd in r1
		"mov r2, r5\n"							// Store arg in r2
		"mov r3, r7\n"							// Store len in r3
		"bl nex_ioctl_handler_in_c\n"
		"add sp, #364\n"
		"pop {r4-r11, pc}\n"					// return from wlc_ioctl
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