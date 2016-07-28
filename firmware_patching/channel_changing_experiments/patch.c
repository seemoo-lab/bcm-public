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
#include "bcmdhd/include/bcmwifi_channels.h"

int cnt = 0;

void
wlc_channel_set_chanspec_hook_in_c(void *wlc_cm, unsigned short chanspec, int local_constraint_qdbm)
{
	printf("%s: %08x %08x %08x\n", __FUNCTION__, (int) wlc_cm, chanspec, local_constraint_qdbm);
}

__attribute__((naked)) void
wlc_channel_set_chanspec_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl wlc_channel_set_chanspec_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"push {r4-r8,lr}\n"
		"b wlc_channel_set_chanspec_plus4\n"
		);
}

void
wlc_set_chanspec_hook(void *wlc_info, unsigned short chanspec)
{
	printf("%s: %08x %08x\n", __FUNCTION__, (int) wlc_info, chanspec);
	wlc_set_chanspec(wlc_info, chanspec);
}

void
wlc_iovar_change_handler_hook_in_c(void *wlc, int a2, int cmd, char *a4, unsigned int a6, int a7, int a8, int a9, int wlcif)
{
	if (cmd == 237)
	printf("%08x %d\n", (int) wlc, cmd);
}

__attribute__((naked)) void
wlc_iovar_change_handler_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl wlc_iovar_change_handler_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"push {r4-r11,lr}\n"
		"b wlc_iovar_change_handler_plus4\n"
		);
}

int
wlc_set_var_chanspec(struct wlc_info *wlc, unsigned short chanspec, struct wlc_bsscfg *bsscfg)
{
	char bandlocked;
	int ret;

	printf("%s: %d\n", __FUNCTION__, cnt);

	chanspec = 0x100f;

	if(wlc_valid_chanspec_db(wlc->cmi, chanspec)) {
		if (!wlc->pub->up_maybe && wlc->pub->field_30 > 1) {
			bandlocked = wlc->bandlocked;
			if (!bandlocked) {
				if (wlc->band->bandunit != ((chanspec & 0xC000) - 0xC000 <= 0)) {
					bandlocked = 1;
				}
				wlc->field_1CB = bandlocked;
			}
		}
		wlc->field_364[25] = chanspec;
		wlc->some_chanspec = chanspec;
		bsscfg->field_546 = chanspec;
		ret = wlc->pub->up_maybe;
		if (ret) {
			ret = wlc->pub->associated;
			if (ret) {
				ret = 0;
			} else if (wlc_phy_chanspec_get(wlc->band->pi) != chanspec) {
				sub_39DCC(wlc, chanspec);
				wlc_bmac_suspend_mac_and_wait_wrapper(wlc);
				wlc_set_chanspec(wlc, chanspec);
				wlc_enable_mac(wlc);
			}
		}
	} else {
		return -20;
	}
	return ret;
}

__attribute__((naked)) void
wlc_set_var_chanspec_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"mov r0, r5\n" // wlc
		"mov r1, r9\n" // chanspec
		"mov r2, r7\n" // bsscfg
		"bl wlc_set_var_chanspec\n"
		"mov r4, r0\n"
		"pop {r0-r3,lr}\n"
		"mov r0, r4\n"
		"add sp, sp, #0x1bc\n"
		"pop {r4-r11, pc}\n"
		);
}

__attribute__((naked)) int
wlc_valid_chanspec_ext_orig(void *wlc_cm, unsigned short chanspec, int dualband)
{
	asm(
		"push {r3-r9,lr}\n"
		"b wlc_valid_chanspec_ext_plus4\n"
		);
}

/* bandwidth ASCII string */
static const char *wf_chspec_bw_str[] =
{
	"5",
	"10",
	"20",
	"40",
	"80",
	"160",
	"80+80",
	"na"
};

static const uint8 wf_chspec_bw_mhz[] =
{5, 10, 20, 40, 80, 160, 160};

#define WF_NUM_BW \
	(sizeof(wf_chspec_bw_mhz)/sizeof(uint8))

/* 40MHz channels in 5GHz band */
static const uint8 wf_5g_40m_chans[] =
{38, 46, 54, 62, 102, 110, 118, 126, 134, 142, 151, 159};
#define WF_NUM_5G_40M_CHANS \
	(sizeof(wf_5g_40m_chans)/sizeof(uint8))

/* 80MHz channels in 5GHz band */
static const uint8 wf_5g_80m_chans[] =
{42, 58, 106, 122, 138, 155};
#define WF_NUM_5G_80M_CHANS \
	(sizeof(wf_5g_80m_chans)/sizeof(uint8))

/* 160MHz channels in 5GHz band */
static const uint8 wf_5g_160m_chans[] =
{50, 114};
#define WF_NUM_5G_160M_CHANS \
	(sizeof(wf_5g_160m_chans)/sizeof(uint8))

/* given a chanspec and a string buffer, format the chanspec as a
 * string, and return the original pointer a.
 * Min buffer length must be CHANSPEC_STR_LEN.
 * On error return NULL
 */
char *
wf_chspec_ntoa(unsigned short chspec, char *buf)
{
	const char *band;
	uint ctl_chan;

	//if (wf_chspec_malformed(chspec))
	//	return NULL;

	band = "";

	/* check for non-default band spec */
	if ((CHSPEC_IS2G(chspec) && CHSPEC_CHANNEL(chspec) > CH_MAX_2G_CHANNEL) ||
	    (CHSPEC_IS5G(chspec) && CHSPEC_CHANNEL(chspec) <= CH_MAX_2G_CHANNEL))
		band = (CHSPEC_IS2G(chspec)) ? "2g" : "5g";

	/* ctl channel */
	ctl_chan = fw_wf_chspec_ctlchan(chspec);

	/* bandwidth and ctl sideband */
	if (CHSPEC_IS20(chspec)) {
		sprintf(buf, "%s%d", band, ctl_chan);
	} else if (!CHSPEC_IS8080(chspec)) {
		const char *bw;
		const char *sb = "";

		bw = wf_chspec_bw_str[(chspec & WL_CHANSPEC_BW_MASK) >> WL_CHANSPEC_BW_SHIFT];

#ifdef CHANSPEC_NEW_40MHZ_FORMAT
		/* ctl sideband string if needed for 2g 40MHz */
		if (CHSPEC_IS40(chspec) && CHSPEC_IS2G(chspec)) {
			sb = CHSPEC_SB_UPPER(chspec) ? "u" : "l";
		}

		sprintf(buf, "%s%d/%s%s", band, ctl_chan, bw, sb);
#else
		/* ctl sideband string instead of BW for 40MHz */
		if (CHSPEC_IS40(chspec)) {
			sb = CHSPEC_SB_UPPER(chspec) ? "u" : "l";
			sprintf(buf, "%s%d%s", band, ctl_chan, sb);
		} else {
			sprintf(buf, "%s%d/%s", band, ctl_chan, bw);
		}
#endif /* CHANSPEC_NEW_40MHZ_FORMAT */

	} else {
		/* 80+80 */
		uint chan1 = (chspec & WL_CHANSPEC_CHAN1_MASK) >> WL_CHANSPEC_CHAN1_SHIFT;
		uint chan2 = (chspec & WL_CHANSPEC_CHAN2_MASK) >> WL_CHANSPEC_CHAN2_SHIFT;

		/* convert to channel number */
		chan1 = (chan1 < WF_NUM_5G_80M_CHANS) ? wf_5g_80m_chans[chan1] : 0;
		chan2 = (chan2 < WF_NUM_5G_80M_CHANS) ? wf_5g_80m_chans[chan2] : 0;

		/* Outputs a max of CHANSPEC_STR_LEN chars including '\0'  */
		sprintf(buf, "%d/80+80/%d-%d", ctl_chan, chan1, chan2);
	}

	return (buf);
}

int
wlc_valid_chanspec_ext_hook_in_c(void *wlc_cm, unsigned short chanspec, int dualband, int lr)
{
	int ret = wlc_valid_chanspec_ext_orig(wlc_cm, chanspec, dualband);
	char buf[20] = { 0 };
	int off = 881;

	if (dualband == 1 && (lr == 0x1ae469 || lr == 0x1ae4af || lr == 0x1ae4ed || lr == 0x1ae521)) {
		cnt++;
		if((cnt < (off+40)) && (cnt >= off)) {
			wf_chspec_ntoa(chanspec, buf);
			printf("A %d %d %04x %s\n", cnt, ret, chanspec, buf);
		}
	}

	if ((chanspec == 0x100e || chanspec == 0x100f) && dualband == 1) {
		//printf("%04x %08x %d\n", chanspec, lr, ret);
		ret = 1;
	}

	return ret;
}

__attribute__((naked)) void
wlc_valid_chanspec_ext_hook(void)
{
	asm(
		"mov r3, lr\n"
		"b wlc_valid_chanspec_ext_hook_in_c\n"
		);
}

void
phy_read_reg_hook_in_c(void *a1, unsigned short a2)
{
	//printf("%s: %08x %04x\n", __FUNCTION__, (int) a1, a2);
	//cnt++;
}

__attribute__((naked)) void
phy_read_reg_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl phy_read_reg_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"mov r2, 0\n"
		"ldr r3, [r0, #0xc4]\n"
		"strh r1, [r3, #0x3fc]\n"
		"strh r2, [r0, #0x168]\n"
		"ldrh r0, [r3, #0x3fe]\n"
		"uxth r0, r0\n"
		"bx lr\n"
		);
}

void
phy_write_reg_hook_in_c(void *a1, int a2, int a3)
{
	//printf("%s: %08x %04x\n", __FUNCTION__, (int) a1, a2);
	//cnt++;
}

__attribute__((naked)) void
phy_write_reg_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl phy_write_reg_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"orr r2, r1, r2, lsl#16\n"
		"ldr r3, [r0, #0xc4]\n"
		"str r2, [r3, #0x3fc]\n"
		"bx lr\n"
		);
}

void
write_radio_reg_hook_in_c(void *a1, int a2, int a3)
{
	//printf("%s: %08x %04x\n", __FUNCTION__, (int) a1, a2);
	//cnt++;
}

__attribute__((naked)) void
write_radio_reg_hook(void)
{
	asm(
		"push {r0-r3,lr}\n"
		"bl write_radio_reg_hook_in_c\n"
		"pop {r0-r3,lr}\n"
		"ldr r3, [r0,#0xc4]\n"
		"strh r1, [r3, #0x3d8]\n"
		"strh r2, [r3, #0x3da]\n"
		"bx lr\n"
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