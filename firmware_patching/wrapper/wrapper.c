
void *dma_rx(void *di) { return 0; }
void *dma_rxfill(void *di) { return 0; }
void *dma_txfast(void *di, void *p, int commit) { return 0; }
void *dngl_sendpkt(void *sdio, void *p, int chan) { return 0; }
void *wlc_mctrl_write(void *wlc_hw_info_ptr) { return 0; }
void *wlc_bmac_mctrl(void *wlc_hw_info_ptr, int mask, int val) { return 0; }
int printf(const char *format, ...) { return 0; }
void *pkt_buf_get_skb(void *osh, unsigned int len) { return 0; }
void *pkt_buf_free_skb(void *osh, void *p, int send) { return 0; }

