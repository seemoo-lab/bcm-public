#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */

void*
filter_handler(void* osh, void* p, int send) {
	printf("filter handler hook: %d!\n", get_register(3));

	return pkt_buf_free_skb(osh, p, send);
}
