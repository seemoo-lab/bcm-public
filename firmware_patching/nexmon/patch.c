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
#include <bcmdhd/bcmsdpcm.h>
#include <bcmdhd/bcmcdc.h>
#include "bcmdhd/include/bcmwifi_channels.h"
#include "ieee80211_radiotap.h"
#include "radiotap.h"
#include "d11.h"

#define AT(addr) __attribute__((at(addr)))

#define BLPatch(addr, func) \
    __attribute__((naked, at(#addr))) void \
    bl_ ## addr(void) { asm("bl " #func "\n"); }

#define BPatch(addr, func) \
    __attribute__((naked, at(#addr))) void \
    b_ ## addr(void) { asm("b " #func "\n"); }

#define GenericPatch4(addr, val) \
    __attribute__((at(#addr))) const unsigned int gp4_ ## addr = (unsigned int) (val);

#define GenericPatch2(addr, val) \
    __attribute__((at(#addr))) unsigned short gp2_ ## addr = (unsigned short) (val);

#define GenericPatch1(addr, val) \
    __attribute__((at(#addr))) unsigned char gp1_ ## addr = (unsigned char) (val);

#define StringPatch(addr, val) \
    __attribute__((naked,at(#addr))) void str_ ## addr(void) { asm(".ascii \"" val "\"\n.byte 0x00"); }

#define Dummy(addr) \
    void dummy_ ## addr(void) { ; }

/**
 *  Contains tinflate_partial and other functions needed for deflate decompression
 */
#include "ucode_compression_code.c"

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
 *  rxextheadroom size to have enough space in the sk_buff of a received frame to 
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

/**
 *  remove data to the start of a buffer
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
        printf("rtap_init error\n");
        return 0;
    } else {
        //printf("rtap_init ok, rtap_len: %d\n", rtap_len);
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

#define NEXMON_CTRL_SET_MONITOR 0
#define NEXMON_CTRL_GET_MONITOR 1
#define NEXMON_CTRL_SET_PROMISC 2
#define NEXMON_CTRL_GET_PROMISC 3

// some useful ioctls
#define WLC_GET_PROMISC             9
#define WLC_SET_PROMISC             10
#define WLC_GET_MONITOR             107
#define WLC_SET_MONITOR             108


struct nexmon_ctrl_frame {
    unsigned char cmd;
    unsigned char value;
} __attribute__((packed));

void *
handle_nexmon_ctrl(struct wlc_info* wlc, struct sk_buff *p)
{
    struct osl_info* osh = wlc->osh;
    struct nexmon_ctrl_frame *frm;
    unsigned int value = 0;
    
    // remove bdc header and nexmon control header: "NEXMONNEXMON" 0xff 0xff
    skb_pull(p, 4 + 14);

    frm = p->data;
    value = frm->value;

    switch(frm->cmd) {
        case NEXMON_CTRL_SET_MONITOR:
            printf("set monitor %d\n", frm->value);
            wlc_ioctl(wlc, WLC_SET_MONITOR, &value, 4, wlc);
            break;
        case NEXMON_CTRL_GET_MONITOR:
            printf("get monitor\n");
            break;
        case NEXMON_CTRL_SET_PROMISC:
            printf("set promisc %d\n", frm->value);
            wlc_ioctl(wlc, WLC_SET_PROMISC, &value, 4, wlc);
            break;
        case NEXMON_CTRL_GET_PROMISC:
            printf("get promisc\n");
            break;
        default:
            break;
    }

    return pkt_buf_free_skb(osh, p, 0);
}

void *
handle_sdio_xmit_request_hook(void *sdio_hw, struct sk_buff *p)
{
    struct wl_info *wl = *(*((struct wl_info ***) sdio_hw + 15) + 6);
    struct wlc_info *wlc = wl->wlc;

    if (!strncmp(p->data + 4, "NEXMONNEXMON", 12)) {
        // handle nexmon control frame
        return handle_nexmon_ctrl(wlc, p);
    } else if (wlc->monitor && (*(short *) p->data == 0)) {
        printf("inj: %08x\n", *(int *) p->data);
        // check if in monitor mode, if yes, inject frame
        return inject_frame(wlc, p);
    } else {
        // otherwise, handle frame normally
        return handle_sdio_xmit_request_ram(sdio_hw, p);
    }
}

//BLPatch(0x1f4f08, wlc_ucode_write_compressed);

// Hook the call to wl_monitor in wlc_monitor
#if NEXMON_FW_VERSION == FW_VER_6_37_32_RC23_34_43_r639704
BLPatch(0x18DB20, wl_monitor_hook);
#else
BLPatch(0x18DA30, wl_monitor_hook);
#endif

// Hook the call to dma_attach in wlc_bmac_attach_dmapio
#if NEXMON_FW_VERSION == FW_VER_6_37_32_RC23_34_43_r639704
BLPatch(0x1F4FDA, dma_attach_hook);
#else
BLPatch(0x1F4FCE, dma_attach_hook);
#endif

// Hook the call to handle_sdio_xmit_request_hook in sdio_header_parsing_from_sk_buff
BPatch(0x182AAA, handle_sdio_xmit_request_hook);

// Replace the entry in the function pointer table by handle_sdio_xmit_request_hook
GenericPatch4(0x180BCC, handle_sdio_xmit_request_hook + 1);

// Patch the "wl%d: Broadcom BCM%04x 802.11 Wireless Controller %s\n" string
#if NEXMON_FW_VERSION == FW_VER_6_37_32_RC23_34_43_r639704
StringPatch(0x1FD327, "nexmon (" __DATE__ " " __TIME__ ")\n");
#else
StringPatch(0x1FD31B, "nexmon (" __DATE__ " " __TIME__ ")\n");
#endif

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
Dummy(0x180800);

/**
 *  Just inserted to produce an error while linking, when we try to overwrite memory used by the original firmware
 */
Dummy(0x1AAEB4);
