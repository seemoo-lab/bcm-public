
int bus_binddev(void *sdio_hw, void *sdiodev, void *d11dev) { return 0; } // 0x1837F8
int bus_binddev_rom(void *sdiodev, void *d11dev) { return 0; } // 0x1B8C4

void *dma_rx(void *di) { return 0; } // 0x8c69c
void *dma_rxfill(void *di) { return 0; } // 0x8c6cc
void *dma_txfast(void *di, void *p, int commit) { return 0; } // 0x1844B2
void *dngl_sendpkt(void *sdio, void *p, int chan) { return 0; } // 0x182750

void free(void *p) { ; } // 0x16620

void *malloc(unsigned int size, char x) { return 0; } // 0x1814F4
int memcpy(void *dst, void *src, int len) { return 0; } // 0x181418
void *memset(void *dst, int value, int len) { return 0; } // 0x1269C

void *pkt_buf_get_skb(void *osh, unsigned int len) { return 0; } // 0x184F14
void *pkt_buf_free_skb(void *osh, void *p, int send) { return 0; } // 0x184F64
int printf(const char *format, ...) { return 0; } // 0x126f0

void *setup_some_stuff(void *wl, int vendor, int a3, int a4, char a5, void *osh, int a7, int a8, int a9, int a10) { return 0; } // 0x1F319C
void sub_166b4(void) { ; } // 0x166b4
void sub_16D8C(int a1, int a2, void *a3) { ; } // 0x16D8C
void *sub_1831A0(void *osh, void *a2, int a3, void *sdiodev) { return 0; } // 0x1831A0
void sub_1ECAB0(int a1) { ; } // 0x1ECAB0
int sum_buff_lengths(void *osh, void *p) { return 0; } // 0x1360C

void wlc_bmac_init(void *wlc_hw, unsigned int chanspec, unsigned int mute) { ; } // 0x1AB840
void wlc_bmac_mctrl(void *wlc_hw, int mask, int val) { ; } // 0x4F080
void wlc_bmac_write_template_ram(void *wlc_hw, int offset, int len, void *buf) { ; } // 0x504B0
void wlc_coreinit(void *wlc_hw) { ; } // 0x1AB66C
int wlc_init(void *wlc) { return 0; } // 0x199874
void wlc_mctrl_write(void *wlc_hw) { ; } // 0x4df60
void wlc_txfifo(void *wlc, int fifo, void *p, void *txh, unsigned char commit, char txpktpend) { ; } // 0x193744
void wlc_ucode_write(void *wlc_hw, const int ucode[], const unsigned int nbytes) { ; } // 0x4E0C8

void *wl_add_if(void *wl, int wlcif, int unit, int wds_or_bss) { return 0; } // 0x26F50
void *wl_alloc_if(void *wl, int iftype, int unit, int wlc_if) { return 0; } // 0x271B0
int wl_init(void *wl) { return 0; } // 0x2716C
int wl_reset(void *wl) { return 0; } // 0x27138

void *sdio_header_parsing_from_sk_buff(void *sdio, void *p) { return 0; } //0x182A64

