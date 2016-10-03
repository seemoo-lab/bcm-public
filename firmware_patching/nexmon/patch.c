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
    ret = wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, data_rate, 0);
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

int
wlc_ioctl_hook(void *wlc, int cmd, void *arg, int len, void *wlc_if)
{
    int ret;

    switch(cmd) {
        case 12:
            //printf("WLC_GET_RATE");
            break;
        case 23:
            //printf("WLC_GET_BSSID");
            break;
        case 49:
            printf("WLC_SET_PASSIVE_SCAN %08x\n", *(int *) arg);
            break;
        case 127:
            //printf("WLC_GET_RSSI");
            break;
        case 137:
            //printf("WLC_GET_PKTCNTS");
            break;
        case 262:
            //printf("get %s", (char *) arg);
            break;
        case 263:
            //printf("set %s", (char *) arg);
            break;
        default:
            //printf("ioctl: %d", cmd);
            break;
    }
    //printf(" len %d\n", len);

    ret = wlc_ioctl(wlc, cmd, arg, len, wlc_if);
/*
    if (cmd == 263 && !strncmp(arg, "escan", 5)) {
        printf("escan %s %d\n", (char *) arg, len);
        argi = (int *) arg;
        for (i = 0; i < len / 4; i++) {
            printf("%08x ", argi[i]);
        }
        printf("\n");
    }
*/

    return ret;
}

void
wlc_custom_scan_hook(void)
{
    printf("%s\n", __FUNCTION__);
}

void
wlc_scan_hook(void *wlc_scan, int bss_type, char *bssid, int nssid, void *ssids)
{
    printf("%s: %d %d\n", __FUNCTION__, bss_type, nssid);
}

struct wlc_bss_info
{
    uint8       BSSID[6];    /* network BSSID */
    uint16      flags;      /* flags for internal attributes */
    uint8       SSID_len;   /* the length of SSID */
    uint8       SSID[32];   /* SSID string */
    int16       RSSI;       /* receive signal strength (in dBm) */
    int16       SNR;        /* receive signal SNR in dB */
    uint16      beacon_period;  /* units are Kusec */
    uint16      atim_window;    /* units are Kusec */
    uint16      chanspec;   /* Channel num, bw, ctrl_sb and band */
    int8        infra;      /* 0=IBSS, 1=infrastructure, 2=unknown */
    void        *rateset;    /* supported rates */
    uint8       dtim_period;    /* DTIM period */
    int8        phy_noise;  /* noise right after tx (in dBm) */
    uint16      capability; /* Capability information */
/*
#ifdef WLSCANCACHE
    uint32      timestamp;  // in ms since boot, OSL_SYSUPTIME()
#endif
    struct dot11_bcn_prb *bcn_prb; // beacon/probe response frame (ioctl na)
    uint16      bcn_prb_len;    // beacon/probe response frame length (ioctl na)
    uint8       wme_qosinfo;    // QoS Info from WME IE; valid if WLC_BSS_WME flag set
    struct rsn_parms wpa;
    struct rsn_parms wpa2;
#ifdef BCMWAPI_WAI
    struct rsn_parms wapi;
#endif // BCMWAPI_WAI
#if defined(WLP2P)
    uint32      rx_tsf_l;   // usecs, rx time in local TSF
#endif
    uint16      qbss_load_aac;  // qbss load available admission capacity
    // qbss_load_chan_free <- (0xff - channel_utilization of qbss_load_ie_t)
    uint8       qbss_load_chan_free;    // indicates how free the channel is
    uint8       mcipher;    // multicast cipher
    uint8       wpacfg;     // wpa config index
    uint16      mdid;       // mobility domain id
    uint16      flags2;     // additional flags for internal attributes
*/
};

struct wlc_bss_list {
    unsigned int count;
    unsigned char beacon;
    struct wlc_bss_info *ptrs[64];
};

/*
//__attribute__((at("patch", "", CHIP_VER_BCM4339, FW_VER_ALL)))
void
kaka(void) {
    printf("x\n");
}

//__attribute__((at("patch", "", CHIP_VER_BCM4339, FW_VER_ALL)))
__attribute__((naked))
void
printf_hook(const char * format, ...) {
  asm(
    "push {r0-r3,lr}\n"
    "bl kaka\n"
    "pop {r0-r3,lr}\n"
    "b printf\n"
    );
}

void
printf_hook(const char * format, ...);
*/

void
wlc_custom_scan_complete_hook(struct wlc_info *wlc, int status, void *cfg)
{
    struct wlc_bss_list *scan_results = (struct wlc_bss_list *) wlc->scan_results;
    struct wlc_bss_list *custom_scan_results = (struct wlc_bss_list *) wlc->custom_scan_results;

    wlc_custom_scan_complete(wlc, status, cfg);

    printf("%s %d %d %d\n", __FUNCTION__, status, scan_results->count, custom_scan_results->count);
    printf("   %08x %08x\n", (int) scan_results->ptrs[0], (int) scan_results->ptrs[1]);
    printf("   %08x %08x\n", (int) custom_scan_results->ptrs[0], (int) custom_scan_results->ptrs[1]);
    printf("   %08x %08x\n", (int) wlc->scan_results, (int) wlc->custom_scan_results);
    printf("   %08x %08x\n", *(int *) wlc->scan_results, *(int *) wlc->custom_scan_results);
}

/*
// Replace address of wlc_custom_scan_complete in wlc_custom_scan
__attribute__((at(0x19164C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x19173C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
GenericPatch4(wlc_custom_scan_complete, wlc_custom_scan_complete_hook + 1);

__attribute__((at(0x191438, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x191528, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
HookPatch4(wlc_custom_scan, wlc_custom_scan_hook, "push {r4-r11,lr}");

__attribute__((at(0x1C9884, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x1C9AD0, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
HookPatch4(wlc_scan, wlc_scan_hook, "push {r4-r11,lr}");

// Replace address of wlc_ioctl in wlc_attach
__attribute__((at(0x1F347C, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_40_r581243)))
__attribute__((at(0x1F3488, "", CHIP_VER_BCM4339, FW_VER_6_37_32_RC23_34_43_r639704)))
GenericPatch4(wlc_ioctl, wlc_ioctl_hook + 1);
*/
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
