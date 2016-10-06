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
 *                                                                         *
 **************************************************************************/

#pragma NEXMON targetregion "patch"

#include <firmware_version.h>   // definition of firmware version macros
#include <debug.h>              // contains macros to access the debug hardware
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <rates.h>              // rates used to build the ratespec for frame injection
#include <bcmdhd/bcmsdpcm.h>
#include <bcmdhd/bcmcdc.h>
#include "bcmdhd/include/bcmwifi_channels.h"
#include "ieee80211_radiotap.h"
#include "d11.h"

struct bdc_radiotap_header {
    struct bdc_header bdc;
    struct nexmon_radiotap_header radiotap;
} __attribute__((packed));

/**
 *  add data to the start of a buffer
 */
void *
skb_push(sk_buff *p, unsigned int len)
{
    p->data -= len;
    p->len += len;

    if (p->data < p->head)
        printf("%s: failed", __FUNCTION__);

    return p->data;
}

/**
 *  remove data from the start of a buffer
 */
void *
skb_pull(sk_buff *p, unsigned int len) {
    p->data += len;
    p->len -= len;

    return p->data;
}

void
wl_monitor_hook(struct wl_info *wl, struct wl_rxsts *sts, struct sk_buff *p)
{
    struct osl_info *osh = wl->wlc->osh;
    void *sdio_info = *(*((void ***) 0x180e60) + 7);
    struct sk_buff *p_new = pkt_buf_get_skb(osh, p->len + sizeof(struct bdc_radiotap_header));
    struct bdc_radiotap_header *frame = (struct bdc_radiotap_header *) p_new->data;
    
    memset(p_new->data, 0, sizeof(struct bdc_radiotap_header));

    frame->bdc.flags = 0x20;
    frame->bdc.priority = 0;
    frame->bdc.flags2 = 0;
    frame->bdc.dataOffset = 0;

    frame->radiotap.header.it_version = 0;
    frame->radiotap.header.it_pad = 0;
    frame->radiotap.header.it_len = sizeof(struct nexmon_radiotap_header);
    frame->radiotap.header.it_present = 
          (1<<IEEE80211_RADIOTAP_TSFT) 
        | (1<<IEEE80211_RADIOTAP_FLAGS)
        | (1<<IEEE80211_RADIOTAP_CHANNEL)
        | (1<<IEEE80211_RADIOTAP_DBM_ANTSIGNAL)
        | (1<<IEEE80211_RADIOTAP_DBM_ANTNOISE);
    frame->radiotap.tsf.tsf_l = sts->mactime;
    frame->radiotap.tsf.tsf_h = 0;
    frame->radiotap.flags = IEEE80211_RADIOTAP_F_FCS;
    frame->radiotap.chan_freq = wlc_phy_channel2freq(CHSPEC_CHANNEL(sts->chanspec));
    frame->radiotap.chan_flags = 0;
    frame->radiotap.dbm_antsignal = sts->signal;
    frame->radiotap.dbm_antnoise = sts->noise;

    memcpy(p_new->data + sizeof(struct bdc_radiotap_header), p->data + 6, p->len - 6);
    p_new->len -= 6;

    dngl_sendpkt(sdio_info, p_new, SDPCM_DATA_CHANNEL);
}

void *
inject_frame(struct wlc_info *wlc, struct sk_buff *p)
{
    int rtap_len = 0;
    int data_rate = 0;

    // Radiotap parsing:
    struct ieee80211_radiotap_iterator iterator;
    struct ieee80211_radiotap_header *rtap_header;

    // remove bdc header
    skb_pull(p, 4);

    // parse radiotap header
    rtap_len = *((char *)(p->data + 2));
    rtap_header = (struct ieee80211_radiotap_header *) p->data;
    
    int ret = ieee80211_radiotap_iterator_init(&iterator, rtap_header, rtap_len);
    
    if(ret) {
        pkt_buf_free_skb(wlc->osh, p, 0);
        printf("rtap_init error\n");
        return 0;
    }

    while(!ret) {
        ret = ieee80211_radiotap_iterator_next(&iterator);
        if(ret) {
            continue;
        }
        switch(iterator.this_arg_index) {
            case IEEE80211_RADIOTAP_RATE:
                data_rate = (*iterator.this_arg);
                break;
            case IEEE80211_RADIOTAP_CHANNEL:
                //printf("Channel (freq): %d\n", iterator.this_arg[0] | (iterator.this_arg[1] << 8) );
                break;
            default:
                //printf("default: %d\n", iterator.this_arg_index);
                break;
        }
    }

    // remove radiotap header
    skb_pull(p, rtap_len);

    if (wlc->band->bandtype == WLC_BAND_5G && data_rate < RATES_RATE_6M) {
        data_rate = RATES_RATE_6M;
        data_rate = RATES_OVERRIDE_MODE | BW_160MHZ | RATES_ENCODE_HT;
        printf("rate: %08x\n", data_rate);
    }
    

    ret = wlc_sendctl(wlc, p, wlc->active_queue, wlc->band->hwrs_scb, 1, data_rate, 0);
    //printf("inj %d %08x\n", ret, data_rate);

    return 0;
}

void *
handle_sdio_xmit_request_hook(void *sdio_hw, struct sk_buff *p)
{
    struct wl_info *wl = *(*((struct wl_info ***) sdio_hw + 15) + 6);
    struct wlc_info *wlc = wl->wlc;

    if (wlc->monitor && p != 0 && p->data != 0 && ((short *) p->data)[2] == 0) {
        // check if in monitor mode and if first two bytes in the frame correspond to a radiotap header, if true, inject frame
        return inject_frame(wlc, p);
    } else {
        // otherwise, handle frame normally
        return handle_sdio_xmit_request_ram(sdio_hw, p);
    }
}


// Hook the call to wlc_ucode_write in wlc_ucode_download
__attribute__((at(0x1F4F08, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x1F4F14, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
BLPatch(wlc_ucode_write_compressed, wlc_ucode_write_compressed);

// Hook the call to wl_monitor in wlc_monitor
__attribute__((at(0x18DA30, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x18DB20, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
BLPatch(wl_monitor_hook, wl_monitor_hook);

// Hook the call to handle_sdio_xmit_request_hook in sdio_header_parsing_from_sk_buff
__attribute__((at(0x182AAA, "", CHIP_VER_BCM4339, FW_VER_ALL)))
BPatch(handle_sdio_xmit_request_hook, handle_sdio_xmit_request_hook);

// Replace the entry in the function pointer table by handle_sdio_xmit_request_hook
__attribute__((at(0x180BCC, "", CHIP_VER_BCM4339, FW_VER_ALL)))
GenericPatch4(handle_sdio_xmit_request_hook, handle_sdio_xmit_request_hook + 1);

// Patch the "wl%d: Broadcom BCM%04x 802.11 Wireless Controller %s\n" string
__attribute__((at(0x1FD31B, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x1FD327, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
StringPatch(version_string, "nexmon (" __DATE__ " " __TIME__ ")\n");
