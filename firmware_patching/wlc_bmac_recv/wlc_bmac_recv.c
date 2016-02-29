#include "../include/bcm4339.h" /* needs to be imported before wrapper.h to define addresses */
#include "../include/wrapper.h"
#include "../include/helper.h"
#include "../include/structs.h"
#include "../include/nexmon_filter.h"

// test if the wlc_pub struct got the correct size, 
// WARNING: compilation fails if not
typedef char assertion_on_mystruct[ (sizeof(struct wlc_pub)==232)*2-1 ];


int wlc_bmac_recv(struct wlc_hw_info *wlc_hw, unsigned int fifo, int bound, int *processed_frame_cnt) {

    struct wlc_pub *pub = wlc_hw->wlc->pub;
    sk_buff *p;
    char is_amsdu = pub->is_amsdu;
    int n = 0;
    int bound_limit;
    if(bound) {
        bound_limit = pub->tunables->rxbnd;
    } else {
        bound_limit = -1;
    }
    do {
        p = dma_rx (wlc_hw->di[fifo]);
        if(!p) {
            goto LEAVE;
        }
        // nexmon filtering
        // 2nd parameter is zero => the filter is currently static
        if( nexmon_filter(p, 0) == 0 ) {
            printf("FILTER: tossed!\n");
            ++n;
            pkt_buf_free_skb(wlc_hw->wlc->osh, p, 0);
            goto LEAVE;
        } else {
            printf("FILTER: keep!\n");
        }

        if(is_amsdu) {
            is_amsdu = 0;
        }
        dngl_sendpkt(SDIO_INFO_ADDR, p, 0xF);
        ++n;
    } while(n < bound_limit);
LEAVE:
    dma_rxfill(wlc_hw->di[fifo]);
    wlc_bmac_mctrl(wlc_hw, 0x41d60000, 0x41d20000);
    *processed_frame_cnt += n;
    if ( n < bound_limit ) {
        return 0;
    } else {
        return 1;
    }
}

