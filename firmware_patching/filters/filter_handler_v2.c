#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */

void*
filter_handler(void *sdio, sk_buff *p) {
    int chan = *((int *)(p->data + 1)) & 0xf;
    //void *data_payload = (void *)(p->data + 8);
    int offset = 0;
    if(*((int *)(sdio + 0x220))) {
        offset = *((int *)(p->data + 3)) - 20;
    } else {
        offset = *((int *)(p->data + 3)) - 12;
    }

    //p->data = data_payload + offset;
    //p->len = p->len - 8 - offset;

    if(chan == 4) {
        printf("our filter!: %d\n", chan);
        hexdump(0, p->data + 8 + offset, p->len - 8 - offset);
        //hexdump(0, p->data, p->len);
    } else {
        printf("some other stuff... %d\n", chan);
    }
    
    return sdio_header_parsing_from_sk_buff(sdio, p);
}
