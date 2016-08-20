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
 * file. To make this work, the function prototypes have to be written as  *
 * one line per prototype. Each prototype has to start with the word       *
 * "extern" and end with a comment containing the functions location in    *
 * memory as a hex number. Before the address, there has to be a space.    *                                                       *
 **************************************************************************/

#ifndef WRAPPER_H
#define WRAPPER_H
#include "../include/structs.h"

extern int ai_setcoreidx(void *sii, unsigned int coreidx); // 0xFE44

extern void before_before_initialize_memory(void); // 0x183886
extern int bus_binddev(void *sdio_hw, void *sdiodev, void *d11dev); // 0x1837F8
extern int bus_binddev_rom(void *sdiodev, void *d11dev); // 0x1B8C4

extern int dma64_txunframed(void *di, void *data, unsigned int len, char commit); // 0x16284
extern void *dma_attach(void *osh, char *name, void* sih, unsigned int dmaregstx, unsigned int dmaregsrx, unsigned int ntxd, unsigned int nrxd, unsigned int rxbufsize, int rxextheadroom, unsigned int nrxpost, unsigned int rxoffset, void *msg_level); // 0x15694
extern void *dma_rx(void *di); // 0x8c69c
extern void *dma_rxfill(void *di); // 0x8c6cc
extern void *dma_txfast(void *di, void *p, int commit); // 0x1844B2
extern void *dma_txfast_plus_4(void *di, void *p, int commit); // 0x1844B6
extern void *dngl_sendpkt(void *sdio, void *p, int chan); // 0x182750

extern void *dump_stack_print_dbg_stuff_intr_handler(void); // 0x181E48

extern void enable_interrupts_and_wait(void); // 0x166B4

extern void free(void *p); // 0x16620
extern int function_with_huge_jump_table(void *wlc, int a2, int cmd, int a4, int a5, unsigned int a6, int a7, int a8, int a9, int a10); // 0x19B25C
extern int fw_wf_chspec_ctlchan(unsigned short chanspec); // 0x136FC

extern int handle_ioctl_cmd(void *wlc, int cmd, void *buf, int len); // 0x2BDBC

extern void *malloc(unsigned int size, char x); // 0x1814F4
extern int memcpy(void *dst, void *src, int len); // 0x181418
extern void *memset(void *dst, int value, int len); // 0x1269C

extern int path_to_load_ucode(int devid, void *osh, void *regs, int bustype, void *sdh); // 0x1FD78C
extern void *pkt_buf_get_skb(void *osh, unsigned int len); // 0x184F14
extern void *pkt_buf_free_skb(void *osh, void *p, int send); // 0x184F64
extern int printf(const char *format, ...); // 0x126f0

extern int read_radio_reg(void *pi, short addr); // 0x1C3CE8

extern void *sdio_header_parsing_from_sk_buff(void *sdio, void *p); // 0x182A64
extern void setup(void); // 0x181128
extern void *setup_some_stuff(void *wl, int vendor, int a3, int a4, char a5, void *osh, int a7, int a8, int a9, int a10); // 0x1F319C
extern unsigned int si_getcuridx(void *sii); // 0x1d474
extern void si_setcore(void *sii, int coreid, int coreunit); // 0x1DCBC
extern void si_update_chipcontrol_shm(void *sii, int addr, int mask, int data); // 0x184878
extern int sprintf(char *buf, const char *format, ...); // 0x12794
extern int strncmp(char *str1, char *str2, unsigned int num); // 0x1283C
extern void sub_166b4(void); // 0x166b4
extern void sub_16D8C(int a1, int a2, void *a3); // 0x16D8C
extern void *sub_1831A0(void *osh, void *a2, int a3, void *sdiodev); // 0x1831A0
extern void sub_1ECAB0(int a1); // 0x1ECAB0
extern int sum_buff_lengths(void *osh, void *p); // 0x1360C

extern void *sub_39DCC(void *wlc, int chanspec); // 0x39DCC
extern void sub_1ed41c(void); // 0x1ed41c
extern void sub_1810a8(void); // 0x1810a8
extern void sub_1ec7c8(void); // 0x1ec7c8
extern void sub_1ed584(void); // 0x1ed584
extern void sub_1ecab0(void); // 0x1ecab0
extern void sub_1ec6fc(void); // 0x1ec6fc
extern void sub_1816e4(void); // 0x1816e4
extern void *sub_1CECBC(void *a, void *bsscfg, void *p, void *data); // 0x1CECBC

extern int towards_dma_txfast(void *sdio, void *p, int chan); // 0x18256C

//extern int wf_chspec_malformed(unsigned short chanspec); // 0x13778
extern void wlc_bmac_init(void *wlc_hw, unsigned int chanspec, unsigned int mute); // 0x1AB840
extern void wlc_bmac_mctrl(void *wlc_hw, int mask, int val); // 0x4F080
extern short wlc_bmac_read_objmem(void *wlc_hw, unsigned int offset, int sel); // 0x4DB68
extern int wlc_bmac_read_shm(void *wlc_hw, unsigned int offset); // 0x4F79C
extern void wlc_bmac_read_tsf(void *wlc_hw, unsigned int *tsf_l_ptr, unsigned int *tsf_h_ptr); // 0x1AAD84
extern int wlc_bmac_recv(void *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt); // 0x1AAD98
extern int wlc_bmac_recv_plus4(void *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt); // 0x1AAD9C
extern void wlc_bmac_suspend_mac_and_wait_wrapper(void *wlc); // 0x3B0BC
extern void wlc_bmac_suspend_mac_and_wait(void *wlc_hw); // 0x4FC88
extern void *wlc_bmac_write_objmem(void *wlc_hw, unsigned int offset, short v, int sel); // 0x4DCC8
extern void wlc_bmac_write_template_ram(void *wlc_hw, int offset, int len, void *buf); // 0x504B0
extern void *wlc_bsscfg_find_by_wlcif(void *wlc, int wlcif); // 0x1AC166
extern void *wlc_channel_set_chanspec(void *wlc_cm, unsigned short chanspec, int local_constraint_qdbm); // 0x1AE2BC
extern void *wlc_channel_set_chanspec_plus4(void *wlc_cm, unsigned short chanspec, int local_constraint_qdbm); // 0x1AE2C0
extern void *wlc_compute_plcp(void *wlc, unsigned int rspec, short length, short type_subtype_frame_ctl_field, char *plcp); // 0x32A90
extern void wlc_coreinit(void *wlc_hw); // 0x1AB66C
extern int wlc_d11hdrs(void *wlc, void *p, void *scb, int short_preamble, unsigned int frag, unsigned int nfrag, unsigned int queue, int next_frag_len, int key, int rspec_override); // 0x18C4C8
extern void *wlc_enable_mac(void *wlc); // 0x3352C
extern int wlc_init(void *wlc); // 0x199874
extern int wlc_ioctl(void *wlc, int cmd, void *arg, int len, void *wlc_if); // 0x19551C
extern int wlc_iovar_change_handler(void *wlc, int a2, int cmd, char *a4, unsigned int a6, int a7, int a8, int a9, int wlcif); // 0x19B25C
extern int wlc_iovar_change_handler_plus4(void *wlc, int a2, int cmd, char *a4, unsigned int a6, int a7, int a8, int a9, int wlcif); // 0x19B260
extern int wlc_iovar_op(void *wlc, char *varname, void *params, int p_len, void *arg, int len, char set, void *wlcif); // 0x18BB6C
extern void wlc_mctrl_write(void *wlc_hw); // 0x4DF60
extern void wlc_phyreg_enter(void *pi); // 0x1C456E
extern void wlc_phyreg_exit(void *pi); // 0x1C4588
extern int wlc_phy_channel2freq(unsigned int channel); // 0x1C4B40
extern int wlc_phy_chanspec_get(void *ppi); // 0x1C4B36
extern int wlc_phy_chan2freq_nphy_plus6(void *pi, int a2, int a3, int a4); // 0x1B39FE
extern void wlc_phy_rssi_compute(void *pih, void *ctx); // 0x1C553C
extern void wlc_phy_stay_in_carriersearch_phy(void *pi, int enable); // 0x1BE636
extern void wlc_phy_table_read_phy(void *pi, unsigned int id, unsigned int len, unsigned int offset, unsigned int width, void *data); // 0x1BAF88
extern void wlc_phy_table_write_phy(void *pi, unsigned int id, unsigned int len, unsigned int offset, unsigned int width, const void *data); // 0x1B8CD2
extern int wlc_phy_tx_tone_phy(void *pi, int f_kHz, int max_val, char iqmode, char dac_test_mode, char modify_bbmult); // 0x1BE876
extern int wlc_prec_enq_head(void *wlc, void *q, void *pkt, int prec, char head); // 0x35aa0
extern int wlc_prep_pdu(void *wlc, void *p, int *fifo); // 0x191654
extern void wlc_prep_sdu(void *wlc, void *p, int *counter, int *fifo); // 0x191B00
extern void wlc_radio_upd(void *wlc); // 0x19123C
extern void *wlc_scbfindband(void *wlc, void *bsscfg, char *ea, int bandunit); // 0x1C9DBE
extern void *__wlc_scb_lookup(void *wlc, void *bsscfg, char *ea, int bandunit); // 0x1CA496
extern void *wlc_scb_lookup(void *wlc, void *bsscfg, char *ea); // 0x1CA4CE
extern void *wlc_scb_set_bsscfg(void *scb, void *bsscfg); // 0x76900
extern int wlc_sendpkt(void *wlc, void *p, int wlcif); // 0x197A18
extern int wlc_sendctl(void *wlc, void *p, void *qi, void *scb, unsigned int fifo, unsigned int rate_override, char enq_only); // 0x38BB0
extern void wlc_send_q(void *wlc, void *qi); // 0x1926B8
extern void *wlc_sendnulldata(void *wlc, void *bsscfg, unsigned int *ea, int datarate_maybe, int p_field_26, int prio); // 0x193348
extern void wlc_set_chanspec(void *wlc, unsigned short chanspec); // 0x399FC
extern void wlc_txfifo(void *wlc, int fifo, void *p, void *txh, unsigned char commit, char txpktpend); // 0x193744
extern void *wlc_txc_upd(void *wlc); // 0x18BB56
extern void wlc_ucode_download(void *wlc_hw); // 0x1F4EF8
extern void wlc_ucode_write(void *wlc_hw, const int ucode[], const unsigned int nbytes); // 0x4E0C8
extern int wlc_valid_chanspec_db(void *wlc_cm, unsigned short chanspec); // 0x5D3F4
extern int wlc_valid_chanspec_ext(void *wlc_cm, unsigned short chanspec, int dualband); // 0x1ADA64
extern int wlc_valid_chanspec_ext_plus4(void *wlc_cm, unsigned short chanspec, int dualband); // 0x1ADA68
extern void *wlc_wlc_txq_enq(void *wlc, void *scb, void *p, int prec); // 0x1995C8

extern void *wl_add_if(void *wl, int wlcif, int unit, int wds_or_bss); // 0x26F50
extern void *wl_alloc_if(void *wl, int iftype, int unit, int wlc_if); // 0x271B0
extern int wl_init(void *wl); // 0x2716C
extern int wl_reset(void *wl); // 0x27138

extern void *handle_sdio_xmit_request(void *sdio_hw, void *p); // 0x183798

#endif /*WRAPPER_H*/
