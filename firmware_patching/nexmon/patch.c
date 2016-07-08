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
#include "ieee80211_radiotap.h"
#include "radiotap.h"
#include "d11.h"

inline uint16_t
get_unaligned_le16(uint8 *p) {
    return p[0] | p[1] << 8;
}

inline uint32_t
get_unaligned_le32(uint8 *p) {
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

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
              (1<<IEEE80211_RADIOTAP_TSFT)
            | (1<<IEEE80211_RADIOTAP_FLAGS)
            | (1<<IEEE80211_RADIOTAP_CHANNEL)
            | (1<<IEEE80211_RADIOTAP_DBM_ANTSIGNAL)
//          | (1<<IEEE80211_RADIOTAP_VHT)
            | (1<<IEEE80211_RADIOTAP_VENDOR_NAMESPACE);
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

        dngl_sendpkt(SDIO_INFO_ADDR, p, SDPCM_DATA_CHANNEL);
    }

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



void *
skb_pull(sk_buff *p, unsigned int len) {
    p->data += len;
    p->len -= len;

    return p->data;
}

/* see: https://github.com/spotify/linux/blob/master/net/wireless/radiotap.c */

/**
 * ieee80211_radiotap_iterator_init - radiotap parser iterator initialization
 * @iterator: radiotap_iterator to initialize
 * @radiotap_header: radiotap header to parse
 * @max_length: total length we can parse into (eg, whole packet length)
 *
 * Returns: 0 or a negative error code if there is a problem.
 *
 * This function initializes an opaque iterator struct which can then
 * be passed to ieee80211_radiotap_iterator_next() to visit every radiotap
 * argument which is present in the header.  It knows about extended
 * present headers and handles them.
 *
 * How to use:
 * call __ieee80211_radiotap_iterator_init() to init a semi-opaque iterator
 * struct ieee80211_radiotap_iterator (no need to init the struct beforehand)
 * checking for a good 0 return code.  Then loop calling
 * __ieee80211_radiotap_iterator_next()... it returns either 0,
 * -ENOENT if there are no more args to parse, or -1 if there is a problem.
 * The iterator's @this_arg member points to the start of the argument
 * associated with the current argument index that is present, which can be
 * found in the iterator's @this_arg_index member.  This arg index corresponds
 * to the IEEE80211_RADIOTAP_... defines.
 *
 * Radiotap header length:
 * You can find the CPU-endian total radiotap header length in
 * iterator->max_length after executing ieee80211_radiotap_iterator_init()
 * successfully.
 *
 * Alignment Gotcha:
 * You must take care when dereferencing iterator.this_arg
 * for multibyte types... the pointer is not aligned.  Use
 * get_unaligned((type *)iterator.this_arg) to dereference
 * iterator.this_arg for type "type" safely on all arches.
 *
 * Example code:
 * See Documentation/networking/radiotap-headers.txt
 */

int ieee80211_radiotap_iterator_init(
    struct ieee80211_radiotap_iterator *iterator,
    struct ieee80211_radiotap_header *radiotap_header,
    int max_length)
{
	/* Linux only supports version 0 radiotap format */
	if (radiotap_header->it_version)
		return -1;

	/* sanity check for allowed length and radiotap length field */
	if (max_length < get_unaligned_le16((uint8 *) &radiotap_header->it_len))
		return -1;

	iterator->rtheader = radiotap_header;
	iterator->max_length = get_unaligned_le16((uint8 *) &radiotap_header->it_len);
	iterator->arg_index = 0;
	iterator->bitmap_shifter = get_unaligned_le32((uint8 *) &radiotap_header->it_present);
    //The original radiotap header (without sub-header) consists of 8 byte
	iterator->arg = (uint8 *)radiotap_header + sizeof(uint8) * 8;
	iterator->this_arg = 0;

	/* find payload start allowing for extended bitmap(s) */

	if (iterator->bitmap_shifter & (1<<IEEE80211_RADIOTAP_EXT)) {
		while (get_unaligned_le32(iterator->arg) & (1<<IEEE80211_RADIOTAP_EXT)) {
			iterator->arg += sizeof(uint32);

			/*
			 * check for insanity where the present bitmaps
			 * keep claiming to extend up to or even beyond the
			 * stated radiotap header length
			 */

			if (((unsigned long)iterator->arg - (unsigned long)iterator->rtheader) > iterator->max_length)
				return -1;
		}

		iterator->arg += sizeof(uint32);

		/*
		 * no need to check again for blowing past stated radiotap
		 * header length, because ieee80211_radiotap_iterator_next
		 * checks it before it is dereferenced
		 */
	}

	/* we are all initialized happily */

	return 0;
}

/* see: https://github.com/spotify/linux/blob/master/net/wireless/radiotap.c */

/**
 * ieee80211_radiotap_iterator_next - return next radiotap parser iterator arg
 * @iterator: radiotap_iterator to move to next arg (if any)
 *
 * Returns: 0 if there is an argument to handle,
 * -ENOENT if there are no more args or -1
 * if there is something else wrong.
 *
 * This function provides the next radiotap arg index (IEEE80211_RADIOTAP_*)
 * in @this_arg_index and sets @this_arg to point to the
 * payload for the field.  It takes care of alignment handling and extended
 * present fields.  @this_arg can be changed by the caller (eg,
 * incremented to move inside a compound argument like
 * IEEE80211_RADIOTAP_CHANNEL).  The args pointed to are in
 * little-endian format whatever the endianess of your CPU.
 *
 * Alignment Gotcha:
 * You must take care when dereferencing iterator.this_arg
 * for multibyte types... the pointer is not aligned.  Use
 * get_unaligned((type *)iterator.this_arg) to dereference
 * iterator.this_arg for type "type" safely on all arches.
 */

int ieee80211_radiotap_iterator_next(
    struct ieee80211_radiotap_iterator *iterator)
{

	/*
	 * small length lookup table for all radiotap types we heard of
	 * starting from b0 in the bitmap, so we can walk the payload
	 * area of the radiotap header
	 *
	 * There is a requirement to pad args, so that args
	 * of a given length must begin at a boundary of that length
	 * -- but note that compound args are allowed (eg, 2 x u16
	 * for IEEE80211_RADIOTAP_CHANNEL) so total arg length is not
	 * a reliable indicator of alignment requirement.
	 *
	 * upper nybble: content alignment for arg
	 * lower nybble: content length for arg
	 */

	uint8 rt_sizes[] = {
		[IEEE80211_RADIOTAP_TSFT] = 0x88,
		[IEEE80211_RADIOTAP_FLAGS] = 0x11,
		[IEEE80211_RADIOTAP_RATE] = 0x11,
		[IEEE80211_RADIOTAP_CHANNEL] = 0x24,
		[IEEE80211_RADIOTAP_FHSS] = 0x22,
		[IEEE80211_RADIOTAP_DBM_ANTSIGNAL] = 0x11,
		[IEEE80211_RADIOTAP_DBM_ANTNOISE] = 0x11,
		[IEEE80211_RADIOTAP_LOCK_QUALITY] = 0x22,
		[IEEE80211_RADIOTAP_TX_ATTENUATION] = 0x22,
		[IEEE80211_RADIOTAP_DB_TX_ATTENUATION] = 0x22,
		[IEEE80211_RADIOTAP_DBM_TX_POWER] = 0x11,
		[IEEE80211_RADIOTAP_ANTENNA] = 0x11,
		[IEEE80211_RADIOTAP_DB_ANTSIGNAL] = 0x11,
		[IEEE80211_RADIOTAP_DB_ANTNOISE] = 0x11,
		[IEEE80211_RADIOTAP_RX_FLAGS] = 0x22,
		[IEEE80211_RADIOTAP_TX_FLAGS] = 0x22,
		[IEEE80211_RADIOTAP_RTS_RETRIES] = 0x11,
		[IEEE80211_RADIOTAP_DATA_RETRIES] = 0x11,
		/*
		 * add more here as they are defined in
		 * include/net/ieee80211_radiotap.h
		 */
	};

	/*
	 * for every radiotap entry we can at
	 * least skip (by knowing the length)...
	 */

	while (iterator->arg_index < sizeof(rt_sizes)) {
		int hit = 0;
		int pad;

		if (!(iterator->bitmap_shifter & 1))
			goto next_entry; /* arg not present */

		/*
		 * arg is present, account for alignment padding
		 *  8-bit args can be at any alignment
		 * 16-bit args must start on 16-bit boundary
		 * 32-bit args must start on 32-bit boundary
		 * 64-bit args must start on 64-bit boundary
		 *
		 * note that total arg size can differ from alignment of
		 * elements inside arg, so we use upper nybble of length
		 * table to base alignment on
		 *
		 * also note: these alignments are ** relative to the
		 * start of the radiotap header **.  There is no guarantee
		 * that the radiotap header itself is aligned on any
		 * kind of boundary.
		 *
		 * the above is why get_unaligned() is used to dereference
		 * multibyte elements from the radiotap area
		 */

		pad = (((unsigned long)iterator->arg) -
			((unsigned long)iterator->rtheader)) &
			((rt_sizes[iterator->arg_index] >> 4) - 1);

		if (pad)
			iterator->arg +=
				(rt_sizes[iterator->arg_index] >> 4) - pad;

		/*
		 * this is what we will return to user, but we need to
		 * move on first so next call has something fresh to test
		 */
		iterator->this_arg_index = iterator->arg_index;
		iterator->this_arg = iterator->arg;
		hit = 1;

		/* internally move on the size of this arg */
		iterator->arg += rt_sizes[iterator->arg_index] & 0x0f;

		/*
		 * check for insanity where we are given a bitmap that
		 * claims to have more arg content than the length of the
		 * radiotap section.  We will normally end up equalling this
		 * max_length on the last arg, never exceeding it.
		 */
		if (((unsigned long)iterator->arg - (unsigned long)iterator->rtheader) > iterator->max_length)
			return -1;

	next_entry:
		iterator->arg_index++;
		if ((iterator->arg_index & 31) == 0) {
			/* completed current u32 bitmap */
			if (iterator->bitmap_shifter & 1) {
				/* b31 was set, there is more */
				/* move to next u32 bitmap */
				iterator->bitmap_shifter =
				    get_unaligned_le32((uint8 *) iterator->next_bitmap);
				iterator->next_bitmap++;
			} else
				/* no more bitmaps: end */
				iterator->arg_index = sizeof(rt_sizes);
		} else /* just try the next bit */
			iterator->bitmap_shifter >>= 1;

		/* if we found a valid arg earlier, return it now */
		if (hit)
			return 0;
	}

	/* we don't know how to handle any more args, we're done */
	return -2;
}

void*
sdio_handler(void *sdio, sk_buff *p) {
    //do the same as in the original function to get the channel:
    int chan = *((int *)(p->data + 1)) & 0xf;
    int rtap_len = 0;
    uint16* chanspec = 0;
    uint16 band = 0;
    //int data_rate = 0;
    void *scb;

    //needed for sending:
    struct wlc_info *wlc = WLC_INFO_ADDR;
    void *bsscfg = 0;

    //Radiotap parsing:
    struct ieee80211_radiotap_iterator iterator;
    struct ieee80211_radiotap_header *rtap_header;

    //do this to get the data offset:
    int offset = 0;
    if(*((int *)(sdio + 0x220))) {
        offset = *((int *)(p->data + 3)) - 20;
    } else {
        offset = *((int *)(p->data + 3)) - 12;
    }

    if(chan && chan == 2) {

        //see sdio_header_parsing_from_sk_buff()
        p->data = p->data + 8 + offset;
        p->len = p->len - 8 - offset;

        //remove bdc header
        skb_pull(p, 4);

        //parse radiotap header
        rtap_len = *((char *)(p->data + 2));
        rtap_header = (struct ieee80211_radiotap_header *) p->data;
        int ret = ieee80211_radiotap_iterator_init(&iterator, rtap_header, rtap_len);
        if(ret) {
            printf("rtap_init error\n");
        } else {
            printf("rtap_init ok, rtap_len: %d\n", rtap_len);
        }
        while(!ret) {
            //returns != 0 on error
            ret = ieee80211_radiotap_iterator_next(&iterator);
            if(ret) {
                continue;
            }
            switch(iterator.this_arg_index) {
                case IEEE80211_RADIOTAP_RATE:
                    printf("Data Rate: %dMbps\n", (*iterator.this_arg) / 2);
                    break;
                case IEEE80211_RADIOTAP_CHANNEL:
                    printf("Channel (freq): %d\n", iterator.this_arg[0] | (iterator.this_arg[1] << 8) );
                    break;
                default:
                    printf("default: %d\n", iterator.this_arg_index);
                    break;
            }
        }

        //remove radiotap header
        skb_pull(p, rtap_len);
        
        bsscfg = wlc_bsscfg_find_by_wlcif(wlc, 0);
        //not _really_ needed but maybe needed for sending on 5ghz
        chanspec = (uint16 *)(*((int *)(bsscfg + 0x30C)) + 0x32);
        //DELME
        //*chanspec = 0xe02a;
        printf("chanspec in FW1 @ %x: 0x%x\n", chanspec, *chanspec);
        band = (*chanspec & 0xC000) - 0xC000;
        // get station control block (scb) for given mac address
        // band seems to be just 1 on 5Ghz and 0 on 2.4Ghz
        scb = __wlc_scb_lookup(wlc, bsscfg, p->data, band <= 0);
        printf("SCB: 0x%x, band: %d\n", scb, band <= 0);
        // set the scb's bsscfg entry
        wlc_scb_set_bsscfg(scb, bsscfg);
        // send the frame with the lowest possible rate
        // TODO set datarate from radiotap header here: 6th parameter, working values 0-5 == 1-24Mpbs
        wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, 0, 0);

        return 0;
    } else {
        return sdio_header_parsing_from_sk_buff(sdio, p);
    }
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
