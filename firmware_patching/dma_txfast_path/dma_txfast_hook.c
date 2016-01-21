#include "../include/bcm4339.h"
#include "../include/wrapper.h"
#include "../include/structs.h"

#define SDIO_SEQ_NUM 0x108

extern void *dngl_sendpkt_alternative(void *sdio, void *p, int chan);

void
*dma_txfast_hook(void *di, struct sk_buff *p, int commit)
{
	struct sk_buff *p1 = 0;
	struct osl_info *osh = OSL_INFO_ADDR;
//	int i = 0;
//	unsigned char *c = (unsigned char *) p->data;
	void *ret = 0;
	unsigned char *sdio = (unsigned char *) SDIO_INFO_ADDR;

//	int buflen = 0;

//	buflen = sum_buff_lengths(osh, p);

	p1 = pkt_buf_get_skb(osh, 100);
	memcpy(p1->data, "SEEMOO", 6);

	// We transmit the original frame first, as it already contains the correct sequence number
	ret = dma_txfast(di, p, commit);

	// Now we check, if an SDIO DMA transfer was done and fix the sequence number that is increased automatically in the dngl_sendpkt function
	if (di == DMA_INFO_SDIODEV_ADDR && ((unsigned char *) p->data)[4] == sdio[SDIO_SEQ_NUM]) {
		sdio[SDIO_SEQ_NUM]++;
		dngl_sendpkt_alternative(SDIO_INFO_ADDR, p1, 0xE);
		sdio[SDIO_SEQ_NUM]--;
	} else {
		dngl_sendpkt_alternative(SDIO_INFO_ADDR, p1, 0xE);
	}

	printf("X %08x %08x %08x %d\n", (unsigned int) di, (unsigned int) p, (unsigned int) p->data, p->len);

	return ret;
}
