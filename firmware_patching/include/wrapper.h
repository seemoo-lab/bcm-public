#ifndef WRAPPER_H
#define WRAPPER_H

extern void *dma_rx(void *di);
extern void *dma_rxfill(void *di);
extern void *dma_txfast(void *di, void *p, int commit);
extern void *dngl_sendpkt(void *sdio, void *p, int chan);
extern void *wlc_mctrl_write(void *wlc_hw_info_ptr);
extern void *wlc_bmac_mctrl(void *wlc_hw_info_ptr, int mask, int val);
extern int printf(const char *format, ...);

#endif /*WRAPPER_H*/
