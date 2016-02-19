#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */

void*
filter_handler(void *sdio, sk_buff *p) {
    int chan = *((int *)(p->data + 1)) & 0xf;
    if(chan == 4) {
        printf("fuck yea: %d\n", chan);
    } else {
        printf("some other stuff... %d\n", chan);
    }
    
    return sdio_header_parsing_from_sk_buff(sdio, p);
}
