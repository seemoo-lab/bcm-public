#ifndef WRAPPER_H
#define WRAPPER_H

extern int bus_binddev(void *sdio_hw, void *sdiodev, void *d11dev); // 0x1837F8
extern int bus_binddev_rom(void *sdiodev, void *d11dev); // 0x1B8C4

extern void *dma_rx(void *di); // 0x8c69c
extern void *dma_rxfill(void *di); // 0x8c6cc
extern void *dma_txfast(void *di, void *p, int commit); // 0x1844B2
extern void *dngl_sendpkt(void *sdio, void *p, int chan); // 0x182750

extern void free(void *p); // 0x16620

extern void *malloc(unsigned int size, char x); // 0x1814F4
extern int memcpy(void *dst, void *src, int len); // 0x181418
extern void *memset(void *dst, int value, int len); // 0x1269C

extern void *pkt_buf_get_skb(void *osh, unsigned int len); // 0x184F14
extern void *pkt_buf_free_skb(void *osh, void *p, int send); // 0x184F64
extern int printf(const char *format, ...); // 0x126f0

extern void *setup_some_stuff(void *wl, int vendor, int a3, int a4, char a5, void *osh, int a7, int a8, int a9, int a10); // 0x1F319C
extern void sub_166b4(void); // 0x166b4
extern void sub_16D8C(int a1, int a2, void *a3); // 0x16D8C
extern void *sub_1831A0(void *osh, void *a2, int a3, void *sdiodev); // 0x1831A0
extern void sub_1ECAB0(int a1); // 0x1ECAB0
extern int sum_buff_lengths(void *osh, void *p); // 0x1360C

extern void wlc_bmac_init(void *wlc_hw, unsigned int chanspec, unsigned int mute); // 0x1AB840
extern void wlc_bmac_mctrl(void *wlc_hw, int mask, int val); // 0x4F080
extern void wlc_bmac_write_template_ram(void *wlc_hw, int offset, int len, void *buf); // 0x504B0
extern void wlc_coreinit(void *wlc_hw); // 0x1AB66C
extern int wlc_init(void *wlc); // 0x199874
extern void wlc_mctrl_write(void *wlc_hw); // 0x4df60
extern void wlc_txfifo(void *wlc, int fifo, void *p, void *txh, unsigned char commit, char txpktpend); // 0x193744
extern void wlc_ucode_write(void *wlc_hw, const int ucode[], const unsigned int nbytes); // 0x4E0C8

extern void *wl_add_if(void *wl, int wlcif, int unit, int wds_or_bss); // 0x26F50
extern void *wl_alloc_if(void *wl, int iftype, int unit, int wlc_if); // 0x271B0
extern int wl_init(void *wl); // 0x2716C
extern int wl_reset(void *wl); // 0x27138

#endif /*WRAPPER_H*/
