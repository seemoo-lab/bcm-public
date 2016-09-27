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

#include <firmware_version.h>   // definition of firmware version macros
#include <debug.h>              // contains macros to access the debug hardware
#include <wrapper.h>            // wrapper definitions for functions that already exist in the firmware
#include <structs.h>            // structures that are used by the code in the firmware
#include <helper.h>             // useful helper functions
#include <patcher.h>            // macros used to craete patches such as BLPatch, BPatch, ...
#include <bcmdhd/bcmsdpcm.h>
#include <bcmdhd/bcmcdc.h>
#include "bcmdhd/include/bcmwifi_channels.h"
#include "ieee80211_radiotap.h"
#include "radiotap.h"
#include "d11.h"

struct bdc_radiotap_header {
    struct bdc_header bdc;
    struct ieee80211_radiotap_header radiotap;
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
    //struct wlc_d11rxhdr *wlc_rxhdr = (struct wlc_d11rxhdr *) p->data;
    struct tsf tsf;

    // get the TSF REG reading
    wlc_bmac_read_tsf(wl->wlc_hw, &tsf.tsf_l, &tsf.tsf_h);

    memset(p_new->data, 0, sizeof(struct bdc_radiotap_header));

    frame->bdc.flags = 0x20;
    frame->bdc.priority = 0;
    frame->bdc.flags2 = 0;
    frame->bdc.dataOffset = 0;

    frame->radiotap.it_version = 0;
    frame->radiotap.it_pad = 0;
    frame->radiotap.it_len = sizeof(struct ieee80211_radiotap_header);
    frame->radiotap.it_present = 
          (1<<IEEE80211_RADIOTAP_TSFT) 
        | (1<<IEEE80211_RADIOTAP_FLAGS)
        | (1<<IEEE80211_RADIOTAP_CHANNEL)
        | (1<<IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
    frame->radiotap.tsf.tsf_l = tsf.tsf_l;
    frame->radiotap.tsf.tsf_h = tsf.tsf_h;
    frame->radiotap.flags = IEEE80211_RADIOTAP_F_FCS;
    frame->radiotap.chan_freq = wlc_phy_channel2freq(CHSPEC_CHANNEL(sts->chanspec));
    frame->radiotap.chan_flags = 0;
    frame->radiotap.dbm_antsignal = sts->rssi;

    memcpy(p_new->data + sizeof(struct bdc_radiotap_header), p->data + 6, p->len - 6);
    p_new->len -= 6;

    dngl_sendpkt(sdio_info, p_new, SDPCM_DATA_CHANNEL);
}

void *
inject_frame(struct wlc_info *wlc, struct sk_buff *p)
{
    int rtap_len = 0;
    uint16* chanspec = 0;
    uint16 band = 0;
    int data_rate = 0;
    void *scb;

    // needed for sending:
    void *bsscfg = 0;

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
                //printf("Data Rate: %dMbps\n", (*iterator.this_arg) / 2);
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

    bsscfg = wlc_bsscfg_find_by_wlcif(wlc, 0);
    //not _really_ needed but maybe needed for sending on 5ghz
    chanspec = (uint16 *)(*((int *)(bsscfg + 0x30C)) + 0x32);
    //printf("chanspec in FW1 @ 0x%x: 0x%x\n", chanspec, *chanspec);
    band = (*chanspec & 0xC000) - 0xC000;
    // get station control block (scb) for given mac address
    // band seems to be just 1 on 5Ghz and 0 on 2.4Ghz
    scb = __wlc_scb_lookup(wlc, bsscfg, p->data, band <= 0);
    // set the scb's bsscfg entry
    wlc_scb_set_bsscfg(scb, bsscfg);
    // 6th parameter: data rate in 0.5MBit units; 
    // 2,4,11,22 => 802.11b
    // 12,18,24,36,48,72,96,108 => 802.11g
    wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, data_rate, 0);

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

int
wlc_ioctl_hook(void *wlc, int cmd, void *arg, int len, void *wlc_if)
{
//    if (cmd != 262 && cmd != 263) {
//        printf("ioctl: %d\n", cmd);    
//    }
    
    return wlc_ioctl(wlc, cmd, arg, len, wlc_if);
}

// Hook the call to wlc_ucode_write in wlc_ucode_download
__attribute__((at("0x1F4F08", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at("0x1F4F14", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
BLPatch(wlc_ucode_write_compressed, wlc_ucode_write_compressed);

// Hook the call to wl_monitor in wlc_monitor
__attribute__((at("0x18DA30", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at("0x18DB20", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
BLPatch(wl_monitor_hook, wl_monitor_hook);

// Hook the call to handle_sdio_xmit_request_hook in sdio_header_parsing_from_sk_buff
__attribute__((at("0x182AAA", "", CHIP_VER_BCM4339, FW_VER_ALL)))
BPatch(handle_sdio_xmit_request_hook, handle_sdio_xmit_request_hook);

// Replace the entry in the function pointer table by handle_sdio_xmit_request_hook
__attribute__((at("0x180BCC", "", CHIP_VER_BCM4339, FW_VER_ALL)))
GenericPatch4(handle_sdio_xmit_request_hook, handle_sdio_xmit_request_hook + 1);

// Replace address of wlc_ioctl in wlc_attach
__attribute__((at("0x1F347C", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at("0x1F3488", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
GenericPatch4(wlc_ioctl, wlc_ioctl_hook + 1);

// Patch the "wl%d: Broadcom BCM%04x 802.11 Wireless Controller %s\n" string
__attribute__((at("0x1FD31B", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at("0x1FD327", "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
StringPatch(version_string, "nexmon (" __DATE__ " " __TIME__ ")\n");

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
__attribute__((at("0x180800", "dummy_keep", CHIP_VER_BCM4339, FW_VER_ALL)))
Dummy(0x180800);

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
__attribute__((at("0x1AAEB4", "dummy_keep", CHIP_VER_BCM4339, FW_VER_ALL)))
Dummy(0x1AAEB4);
