#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"
#include "../include/helper.h"
#include "../include/types.h" /* needs to be included before bcmsdpcm.h */
#include "../include/bcmdhd/bcmsdpcm.h"
#include "../include/bcmdhd/bcmcdc.h"

#include "bdc_ethernet_ipv6_udp_header_array.h"

#define SDIO_SEQ_NUM 0x108

extern void *dngl_sendpkt_alternative(void *sdio, void *p, int chan);

inline struct sk_buff *
create_frame(unsigned int hooked_fct, unsigned int arg0, unsigned int arg1, unsigned int arg2, void *start_address, unsigned int length) {
	struct sk_buff *p = 0;
	struct osl_info *osh = OSL_INFO_ADDR;
	struct bdc_ethernet_ipv6_udp_header *hdr;
	struct nexmon_header *nexmon_hdr;

	p = pkt_buf_get_skb(osh, sizeof(struct bdc_ethernet_ipv6_udp_header) - 1 + sizeof(struct nexmon_header) - 1 + length);

	// copy headers to target buffer
	memcpy(p->data, bdc_ethernet_ipv6_udp_header_array, bdc_ethernet_ipv6_udp_header_length);

	hdr = p->data;
	hdr->ipv6.payload_length = htons(sizeof(struct udp_header) + sizeof(struct nexmon_header) - 1 + length);
	nexmon_hdr = (struct nexmon_header *) hdr->payload;

	nexmon_hdr->hooked_fct = hooked_fct;
	nexmon_hdr->args[0] = arg0;
	nexmon_hdr->args[1] = arg1;
	nexmon_hdr->args[2] = arg2;

	memcpy(nexmon_hdr->payload, start_address, length);

	return p;
}

void *
dma_txfast_hook(void *di, struct sk_buff *p, int commit)
{
	struct sk_buff *p1 = 0;
	struct sk_buff *p2 = 0;
	void *ret = 0;
	unsigned char *sdio = (unsigned char *) SDIO_INFO_ADDR;

	if (di == DMA_INFO_SDIODEV_ADDR) {
//		p1 = create_frame((unsigned int) &dma_txfast, (unsigned int) di, 0, 0, get_stack_ptr(), BOTTOM_OF_STACK - (unsigned int) get_stack_ptr());
	} else {
		p1 = create_frame((unsigned int) &dma_txfast, (unsigned int) di, 0, 0, get_stack_ptr(), BOTTOM_OF_STACK - (unsigned int) get_stack_ptr());
		p2 = create_frame((unsigned int) &dma_txfast, (unsigned int) di, 0, 0, p->data, p->len);
	}

//	if (di == DMA_INFO_D11FIFO1_ADDR) {
//		p2 = pkt_buf_get_skb((struct osl_info *) OSL_INFO_ADDR, p->len);
//		memcpy(p2->data, p->data, p->len);
//		dma_txfast(di, p2, commit);
//	}

	// We transmit the original frame first, as it already contains the correct sequence number
        ret = dma_txfast(di, p, commit);

	// Now we check, if an SDIO DMA transfer was done and fix the sequence number that is increased automatically in the dngl_sendpkt function
	if (p1 != 0) {
		if (di == DMA_INFO_SDIODEV_ADDR && ((unsigned char *) p->data)[4] == sdio[SDIO_SEQ_NUM]) {
			sdio[SDIO_SEQ_NUM]++;
			dngl_sendpkt_alternative(SDIO_INFO_ADDR, p1, SDPCM_DATA_CHANNEL);
			dngl_sendpkt_alternative(SDIO_INFO_ADDR, p2, SDPCM_DATA_CHANNEL);
			sdio[SDIO_SEQ_NUM]--;
		} else {
			dngl_sendpkt_alternative(SDIO_INFO_ADDR, p1, SDPCM_DATA_CHANNEL);
			dngl_sendpkt_alternative(SDIO_INFO_ADDR, p2, SDPCM_DATA_CHANNEL);
		}
	}

	printf("X %08x %08x %08x %d\n", (unsigned int) di, (unsigned int) p, (unsigned int) p->data, p->len);

	return ret;
}
