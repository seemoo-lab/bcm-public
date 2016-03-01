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
 * About this file:                                                        *
 * This file contains function prototypes for functions that already exist *
 * in the original firmware of the BCM4339 Wifi chip. With this file, we   *
 * intend to build wrapper functions that can be called directly from C    *
 * after including this header file and linking against the resulting      *
 * object file ../wrapper/wrapper.o. The latter is generated using the     *
 * Makefile ../wrapper/Makefile. Besides the object file, it also creates  *
 * the linker file ../wrapper/wrapper.ld which is based on this header     *
 * file. To make this work, the function prototypes have to written as one *
 * line per prototype. Each prototype has to start with the word "extern"  *
 * and end with a comment containing the functions location in memory as a *
 * hex number. Before the address has to be a space.                       *
 **************************************************************************/

#ifndef WRAPPER_H
#define WRAPPER_H

extern int bus_binddev(void *sdio_hw, void *sdiodev, void *d11dev); // 0x1837F8
extern int bus_binddev_rom(void *sdiodev, void *d11dev); // 0x1B8C4

extern void *dma_rx(void *di); // 0x8c69c
extern void *dma_rxfill(void *di); // 0x8c6cc
extern void *dma_txfast(void *di, void *p, int commit); // 0x1844B2
extern void *dngl_sendpkt(void *sdio, void *p, int chan); // 0x182750

extern void free(void *p); // 0x16620
extern int function_with_huge_jump_table(void *wlc, int a2, int cmd, int a4, int a5, unsigned int a6, int a7, int a8, int a9, int a10); // 0x19B25C

extern int handle_ioctl_cmd(void *wlc, int cmd, void *buf, int len); // 0x2BDBC

extern void *malloc(unsigned int size, char x); // 0x1814F4
extern int memcpy(void *dst, void *src, int len); // 0x181418
extern void *memset(void *dst, int value, int len); // 0x1269C

extern void *pkt_buf_get_skb(void *osh, unsigned int len); // 0x184F14
extern void *pkt_buf_free_skb(void *osh, void *p, int send); // 0x184F64
extern int printf(const char *format, ...); // 0x126f0

extern void *sdio_header_parsing_from_sk_buff(void *sdio, void *p); // 0x182A64
extern void *setup_some_stuff(void *wl, int vendor, int a3, int a4, char a5, void *osh, int a7, int a8, int a9, int a10); // 0x1F319C
extern void sub_166b4(void); // 0x166b4
extern void sub_16D8C(int a1, int a2, void *a3); // 0x16D8C
extern void *sub_1831A0(void *osh, void *a2, int a3, void *sdiodev); // 0x1831A0
extern void sub_1ECAB0(int a1); // 0x1ECAB0
extern int sum_buff_lengths(void *osh, void *p); // 0x1360C

extern void sub_1ed41c(void); // 0x1ed41c
extern void sub_1810a8(void); // 0x1810a8
extern void sub_1ec7c8(void); // 0x1ec7c8
extern void sub_1ed584(void); // 0x1ed584
extern void sub_1ecab0(void); // 0x1ecab0
extern void sub_1ec6fc(void); // 0x1ec6fc
extern void sub_1816e4(void); // 0x1816e4

extern void wlc_bmac_init(void *wlc_hw, unsigned int chanspec, unsigned int mute); // 0x1AB840
extern void wlc_bmac_mctrl(void *wlc_hw, int mask, int val); // 0x4F080
extern void wlc_bmac_write_template_ram(void *wlc_hw, int offset, int len, void *buf); // 0x504B0
extern void wlc_coreinit(void *wlc_hw); // 0x1AB66C
extern int wlc_init(void *wlc); // 0x199874
extern void wlc_mctrl_write(void *wlc_hw); // 0x4DF60
extern void wlc_txfifo(void *wlc, int fifo, void *p, void *txh, unsigned char commit, char txpktpend); // 0x193744
extern void wlc_ucode_download(void *wlc_hw); // 0x1F4EF8
extern void wlc_ucode_write(void *wlc_hw, const int ucode[], const unsigned int nbytes); // 0x4E0C8

extern void *wl_add_if(void *wl, int wlcif, int unit, int wds_or_bss); // 0x26F50
extern void *wl_alloc_if(void *wl, int iftype, int unit, int wlc_if); // 0x271B0
extern int wl_init(void *wl); // 0x2716C
extern int wl_reset(void *wl); // 0x27138

#endif /*WRAPPER_H*/
