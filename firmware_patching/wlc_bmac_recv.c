#include "bcm4339.h" /* needs to be imported before wrapper.h to define addresses */
#include "wrapper.h"
#include "helper.h"
#include "structs.h"

int wlc_bmac_recv(struct wlc_hw_info *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt) {
    void *p;
    int n = 0;
    int bound_limit = wlc_hw->wlc->pub->tunables->rxbnd;
    do {
        p = dma_rx (wlc_hw->di[fifo]);
        if(p) {
            dngl_sendpkt(SDIO_INFO_ADDR, p, 0xF);
        }
        ++n;
    } while(n < bound_limit);  
    dma_rxfill(wlc_hw->di[fifo]);
    wlc_bmac_mctrl(wlc_hw, 0x41d60000, 0x41d20000);
    *processed_frame_cnt += n;
    if(n < bound_limit) {
        return 0;
    } else {
        return 1;
    }
}
