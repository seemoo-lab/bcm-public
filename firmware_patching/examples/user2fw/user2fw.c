#include <bcm4339.h>
#include <wrapper.h>
#include <structs.h>
#include <helper.h>
#include <types.h> /* needs to be included before bcmsdpcm.h */

void*
sdio_handler(void *sdio, sk_buff *p) {

    //do the same as in the original function to get the channel:
    int chan = *((int *)(p->data + 1)) & 0xf;

    //do this to get the data offset:
    //int offset = 0;
    //if(*((int *)(sdio + 0x220))) {
    //    offset = *((int *)(p->data + 3)) - 20;
    //} else {
    //    offset = *((int *)(p->data + 3)) - 12;
    //}


    if(chan == 4) {
        printf("our data! chan: %d\n", chan);
    } else {
        printf("some other stuff... %d\n", chan);
    }
    
    return sdio_header_parsing_from_sk_buff(sdio, p);
}
