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
#include "bcmdhd/include/bcmwifi_channels.h"
#include "radiotap.h"
#include "d11.h"


struct bdc_radiotap_header {
	struct bdc_header bdc;
	struct ieee80211_radiotap_header radiotap;
} __attribute__((packed));

 /**
 *  This hook is used to change parameters on calls to dma_attach. to increse the 
 *  rxextheadroom size to have enough place in the sk_buff of a received frame to 
 *  append bdc and radiotap headers
 */
struct dma_info*
dma_attach_hook(void *osh, char *name, void* sih, unsigned int dmaregstx, unsigned int dmaregsrx, unsigned int ntxd, unsigned int nrxd, unsigned int rxbufsize, int rxextheadroom, unsigned int nrxpost, unsigned int rxoffset, void *msg_level)
{
    return dma_attach(osh, name, sih, dmaregstx, dmaregsrx, ntxd, nrxd, rxbufsize, 512, nrxpost, rxoffset, msg_level);
}

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

void
wlc_bmac_recv_helper(struct wlc_hw_info *wlc_hw, unsigned int fifo, sk_buff *head, struct tsf *tsf)
{
    int mpc = 0;
    struct bdc_radiotap_header *frame;
    struct wlc_d11rxhdr *wlc_rxhdr;
    sk_buff *p;

    // process each frame
    while ((p = head)) {
        head = head->prev;
        p->prev = 0;

        wlc_rxhdr = (struct wlc_d11rxhdr *) p->data;

        wlc_phy_rssi_compute(wlc_hw->band->pi, &wlc_rxhdr->rxhdr);

        skb_push(p, sizeof(struct bdc_radiotap_header));

        frame = (struct bdc_radiotap_header *) p->data;

        frame->bdc.flags = 0x20;
        frame->bdc.priority = 0;
        frame->bdc.flags2 = 0;
        frame->bdc.dataOffset = 0;

        frame->radiotap.it_version = 0;
        frame->radiotap.it_pad = 0;
        frame->radiotap.it_len = sizeof(struct ieee80211_radiotap_header) + ((wlc_rxhdr->rxhdr.RxStatus1 && RXS_PBPRES) ? 48 : 46);
        frame->radiotap.it_present = 
              IEEE80211_RADIOTAP_TSFT
            | IEEE80211_RADIOTAP_FLAGS
            | IEEE80211_RADIOTAP_CHANNEL
            | IEEE80211_RADIOTAP_DBM_ANTSIGNAL
//          | IEEE80211_RADIOTAP_VHT
            | IEEE80211_RADIOTAP_VENDOR_NAMESPACE;
        frame->radiotap.tsf.tsf_l = tsf->tsf_l;
        frame->radiotap.tsf.tsf_h = tsf->tsf_h;
        frame->radiotap.flags = IEEE80211_RADIOTAP_F_FCS;
        frame->radiotap.chan_freq = wlc_phy_channel2freq(CHSPEC_CHANNEL(wlc_rxhdr->rxhdr.RxChan));
        frame->radiotap.chan_flags = 0;
        frame->radiotap.dbm_antsignal = wlc_rxhdr->rssi;
        //frame->radiotap.vht_known = IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH;
        //frame->radiotap.vht_bandwidth = ;
        frame->radiotap.vendor_oui[0] = 'N';
        frame->radiotap.vendor_oui[1] = 'E';
        frame->radiotap.vendor_oui[2] = 'X';
        frame->radiotap.vendor_sub_namespace = 0;
        frame->radiotap.vendor_skip_length = ((wlc_rxhdr->rxhdr.RxStatus1 && RXS_PBPRES) ? 48 : 46);

        //printf("%04x\n", (wlc_rxhdr->rxhdr.RxChan));

        dngl_sendpkt(SDIO_INFO_ADDR, p, SDPCM_DATA_CHANNEL);
    }

    //printf("%d + %d = %d\n", wlc_hw->di[fifo]->rxbufsize, wlc_hw->di[fifo]->rxextrahdrroom, wlc_hw->di[fifo]->rxbufsize + wlc_hw->di[fifo]->rxextrahdrroom);

    wlc_bmac_mctrl(wlc_hw, 
        MCTL_PROMISC | 
        MCTL_KEEPBADFCS | 
        MCTL_KEEPCONTROL | 
        MCTL_BCNS_PROMISC, 
        MCTL_PROMISC | 
        //MCTL_KEEPBADFCS | 
        MCTL_KEEPCONTROL | 
        MCTL_BCNS_PROMISC);
    
    wlc_iovar_op(wlc_hw->wlc, "mpc", 0, 0, &mpc, 4, 1, 0);
}

int
wlc_bmac_recv_hook(struct wlc_hw_info *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt)
{
    struct wlc_pub *pub = wlc_hw->wlc->pub;
    sk_buff *p;
    sk_buff *head = 0;
    sk_buff *tail = 0;
    int n = 0;
    int bound_limit = bound ? pub->tunables->rxbnd : -1;
    struct tsf tsf;
  
    // gather received frames
    while ((p = dma_rx(wlc_hw->di[fifo]))) {
        if (!tail) {
            head = tail = p;
        } else {
            tail->prev = p;
            tail = p;
        }

        if(++n >= bound_limit)
            break;
    }

    // get the TSF REG reading
    wlc_bmac_read_tsf(wlc_hw, &tsf.tsf_l, &tsf.tsf_h);

    // post more rxbufs
    dma_rxfill(wlc_hw->di[fifo]);

    wlc_bmac_recv_helper(wlc_hw, fifo, head, &tsf);
    
    *processed_frame_cnt += n;

    return n >= bound_limit;
}

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_180800(void)
{
    ;
}

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_1AAEB4(void)
{
    ;
}
