#ifndef WRAPPER_H
#define WRAPPER_H

extern void *dma_rx(void *di); // 0x8c69c
extern void *dma_rxfill(void *di); // 0x8c6cc
extern void *dma_txfast(void *di, void *p, int commit); // 0x1844B2
extern void *dngl_sendpkt(void *sdio, void *p, int chan); // 0x182750

extern int memcpy(void *dst, void *src, int len); // 0x181418

extern void *pkt_buf_get_skb(void *osh, unsigned int len); // 0x184F14
extern void *pkt_buf_free_skb(void *osh, void *p, int send); // 0x184F64
extern int printf(const char *format, ...); // 0x126f0

extern int sum_buff_lengths(void *osh, void *p); // 0x1360C

extern void *wlc_mctrl_write(void *wlc_hw_info_ptr); // 0x4df60
extern void *wlc_bmac_mctrl(void *wlc_hw_info_ptr, int mask, int val); // 0x4F080

#endif /*WRAPPER_H*/
