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

unsigned char bdc_header_array[] = { 0x20, 0x00, 0x00, 8 };
unsigned char radiotap_header_array[] = { 0x00, 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00 };

struct tsf {
	unsigned int tsf_l;
	unsigned int tsf_h;
} __attribute__((packed));

struct ieee80211_radiotap_header {
	char it_version;
	char it_pad;
	short it_len;
	int it_present;
	struct tsf tsf;
	char flags;
	char PAD;
	unsigned short chan_freq;
	unsigned short chan_flags;
	char dbm_antsignal;
	char PAD;
//	unsigned short vht_known;
//	unsigned char vht_flags;
//	unsigned char vht_bandwidth;
//	unsigned char vht_mcs_nss[4];
//	unsigned char vht_coding;
//	unsigned char vht_group_id;
//	unsigned short vht_partial_aid;
	unsigned char vendor_oui[3];
	unsigned char vendor_sub_namespace;
	unsigned short vendor_skip_length;
} __attribute__((packed));

#define IEEE80211_RADIOTAP_TSFT					(1 << 0)
#define IEEE80211_RADIOTAP_FLAGS				(1 << 1)
#define IEEE80211_RADIOTAP_RATE					(1 << 2)
#define IEEE80211_RADIOTAP_CHANNEL				(1 << 3)
#define IEEE80211_RADIOTAP_FHSS					(1 << 4)
#define IEEE80211_RADIOTAP_DBM_ANTSIGNAL		(1 << 5)
#define IEEE80211_RADIOTAP_DBM_ANTNOISE			(1 << 6)
#define IEEE80211_RADIOTAP_LOCK_QUALITY			(1 << 7)
#define IEEE80211_RADIOTAP_TX_ATTENUATION		(1 << 8)
#define IEEE80211_RADIOTAP_DB_TX_ATTENUATION	(1 << 9)
#define IEEE80211_RADIOTAP_DBM_TX_POWER			(1 << 10)
#define IEEE80211_RADIOTAP_ANTENNA				(1 << 11)
#define IEEE80211_RADIOTAP_DB_ANTSIGNAL			(1 << 12)
#define IEEE80211_RADIOTAP_DB_ANTNOISE			(1 << 13)
#define IEEE80211_RADIOTAP_RX_FLAGS				(1 << 14)
#define IEEE80211_RADIOTAP_TX_FLAGS				(1 << 15)
#define IEEE80211_RADIOTAP_RTS_RETRIES			(1 << 16)
#define IEEE80211_RADIOTAP_DATA_RETRIES			(1 << 17)

#define IEEE80211_RADIOTAP_MCS					(1 << 19)
#define IEEE80211_RADIOTAP_AMPDU_STATUS			(1 << 20)
#define IEEE80211_RADIOTAP_VHT					(1 << 21)

#define IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE	(1 << 29)
#define IEEE80211_RADIOTAP_VENDOR_NAMESPACE		(1 << 30)
#define IEEE80211_RADIOTAP_EXT					(1 << 31)

/* For IEEE80211_RADIOTAP_FLAGS */
#define	IEEE80211_RADIOTAP_F_CFP		0x01	/* sent/received during CFP */
#define	IEEE80211_RADIOTAP_F_SHORTPRE	0x02	/* sent/received with short preamble */
#define	IEEE80211_RADIOTAP_F_WEP		0x04	/* sent/received with WEP encryption */
#define	IEEE80211_RADIOTAP_F_FRAG		0x08	/* sent/received with fragmentation */
#define	IEEE80211_RADIOTAP_F_FCS		0x10	/* frame includes FCS */
#define	IEEE80211_RADIOTAP_F_DATAPAD	0x20	/* frame has padding between 802.11 header and payload (to 32-bit boundary) */
#define IEEE80211_RADIOTAP_F_BADFCS		0x40	/* bad FCS */

/* For IEEE80211_RADIOTAP_RX_FLAGS */
#define IEEE80211_RADIOTAP_F_RX_BADPLCP	0x0002	/* frame has bad PLCP */

/* For IEEE80211_RADIOTAP_TX_FLAGS */
#define IEEE80211_RADIOTAP_F_TX_FAIL	0x0001	/* failed due to excessive retries */
#define IEEE80211_RADIOTAP_F_TX_CTS		0x0002	/* used cts 'protection' */
#define IEEE80211_RADIOTAP_F_TX_RTS		0x0004	/* used rts/cts handshake */
#define IEEE80211_RADIOTAP_F_TX_NOACK	0x0008	/* don't expect an ack */

/* For IEEE80211_RADIOTAP_VHT */
#define IEEE80211_RADIOTAP_VHT_KNOWN_STBC           0x0001
#define IEEE80211_RADIOTAP_VHT_KNOWN_TXOP_PS_NA         0x0002
#define IEEE80211_RADIOTAP_VHT_KNOWN_GI             0x0004
#define IEEE80211_RADIOTAP_VHT_KNOWN_SGI_NSYM_DIS       0x0008
#define IEEE80211_RADIOTAP_VHT_KNOWN_LDPC_EXTRA_OFDM_SYM    0x0010
#define IEEE80211_RADIOTAP_VHT_KNOWN_BEAMFORMED         0x0020
#define IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH          0x0040
#define IEEE80211_RADIOTAP_VHT_KNOWN_GROUP_ID           0x0080
#define IEEE80211_RADIOTAP_VHT_KNOWN_PARTIAL_AID        0x0100

#define IEEE80211_RADIOTAP_VHT_FLAG_STBC            0x01
#define IEEE80211_RADIOTAP_VHT_FLAG_TXOP_PS_NA          0x02
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI             0x04
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI_NSYM_M10_9      0x08
#define IEEE80211_RADIOTAP_VHT_FLAG_LDPC_EXTRA_OFDM_SYM     0x10
#define IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED          0x20

struct bdc_radiotap_header {
	struct bdc_header bdc;
	struct ieee80211_radiotap_header radiotap;
} __attribute__((packed));

struct d11rxhdr {
	unsigned short RxFrameSize;			/* Actual byte length of the frame data received */
	unsigned short PAD;
	unsigned short PhyRxStatus_0;		/* PhyRxStatus 15:0 */
	unsigned short PhyRxStatus_1;		/* PhyRxStatus 31:16 */
	unsigned short PhyRxStatus_2;		/* PhyRxStatus 47:32 */
	unsigned short PhyRxStatus_3;		/* PhyRxStatus 63:48 */
	unsigned short PhyRxStatus_4;		/* PhyRxStatus 79:64 */
	unsigned short PhyRxStatus_5;		/* PhyRxStatus 95:80 */
	unsigned short RxStatus1;			/* MAC Rx status */
	unsigned short RxStatus2;			/* extended MAC Rx status */
	unsigned short RxTSFTime;			/* RxTSFTime time of first MAC symbol + M_PHY_PLCPRX_DLY */
	unsigned short RxChan;				/* gain code, channel radio code, and phy type -> looks like chanspec */
} __attribute__((packed));

 /* ucode RxStatus1: */
#define RXS_BCNSENT             0x8000
#define RXS_SECKINDX_MASK       0x07e0
#define RXS_SECKINDX_SHIFT      5
#define RXS_DECERR              (1 << 4)
#define RXS_DECATMPT            (1 << 3)
/* PAD bytes to make IP data 4 bytes aligned */
#define RXS_PBPRES              (1 << 2)
#define RXS_RESPFRAMETX         (1 << 1)
#define RXS_FCSERR              (1 << 0)

/* ucode RxStatus2: */
#define RXS_AMSDU_MASK          1
#define RXS_AGGTYPE_MASK        0x6
#define RXS_AGGTYPE_SHIFT       1
#define RXS_PHYRXST_VALID       (1 << 8)
#define RXS_RXANT_MASK          0x3
#define RXS_RXANT_SHIFT         12

/* RxChan */
#define RXS_CHAN_40             0x1000
#define RXS_CHAN_5G             0x0800
#define RXS_CHAN_ID_MASK        0x07f8
#define RXS_CHAN_ID_SHIFT       3
#define RXS_CHAN_PHYTYPE_MASK   0x0007
#define RXS_CHAN_PHYTYPE_SHIFT  0

struct wlc_d11rxhdr {
	struct d11rxhdr rxhdr;
	unsigned int tsf_l;
	char rssi;							/* computed instanteneous RSSI in BMAC */
	char rxpwr0;
	char rxpwr1;
	char do_rssi_ma;					/* do per-pkt sampling for per-antenna ma in HIGH */
	char rxpwr[4];						/* rssi for supported antennas */
} __attribute__((packed));

/**
 *	This hook is used to change parameters on calls to dma_attach. to increse the 
 *	rxextheadroom size to have enough place in the sk_buff of a received frame to 
 *	append bdc and radiotap headers
 */
struct dma_info*
dma_attach_hook(void *osh, char *name, void* sih, unsigned int dmaregstx, unsigned int dmaregsrx, unsigned int ntxd, unsigned int nrxd, unsigned int rxbufsize, int rxextheadroom, unsigned int nrxpost, unsigned int rxoffset, void *msg_level)
{
	return dma_attach(osh, name, sih, dmaregstx, dmaregsrx, ntxd, nrxd, rxbufsize, 512, nrxpost, rxoffset, msg_level);
}

/**
 *	add data to the start of a buffer
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
//			| IEEE80211_RADIOTAP_VHT
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

		printf("%04x\n", (wlc_rxhdr->rxhdr.RxChan));

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
 *	Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_180800(void)
{
	;
}

/**
 *	Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
void 
dummy_1AAEB4(void)
{
	;
}