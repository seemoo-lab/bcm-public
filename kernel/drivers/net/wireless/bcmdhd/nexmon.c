#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/types.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <osl.h>

#include "radiotap.h"
#include "nexmon.h"

void
hexdump(unsigned char *msg, int msglen) {
    int i, col; 
    char buf[80];

    ASSERT(strlen(pfx) + 49 <= sizeof(buf));

    col = 0; 

    for (i = 0; i < msglen; i++, col++) {
        if (col % 16 == 0)
        {
            sprintf(buf, "size %i: ", msglen);
            //strcpy(buf, pfx);
        }
        sprintf(buf + strlen(buf), "%02x", msg[i]);
        if ((col + 1) % 16 == 0)
            printf("%s\n", buf);
        else
            sprintf(buf + strlen(buf), " ");
    }    

    if (col % 16 != 0)
        printf("%s\n", buf);
}

struct sk_buff *nexmon_decode(struct sk_buff* skb)
{
	char *data;
	unsigned int data_offset;
	unsigned int pkt_len;
	int rssi = 0;
    ieee80211_radiotap_header_t radiotaph;
    nexmon_radiotap_present_fields_t present_fields;
	
    data = skb->data;
	pkt_len = *(unsigned short*) data;
	skb_trim(skb, pkt_len);
    // offset before the 802.11 header starts
	data_offset = 60;

    //skip packets which are smaller than the 802.11 header
	if(pkt_len < 24) {
        return 0;
    }

    radiotaph.it_version = 0x0;
    radiotaph.it_pad = 0x0;
    radiotaph.it_len = 0xf;
    //present flags
    radiotaph.it_present = 0x2a;
    //flags: FRAME_INC_FCS 
    present_fields.flags = (uint8_t) 0x10;
    present_fields.padding1 = (uint8_t) 0x0;
	// we set the frequency fix for now, this should be delivered by the firmware in the future
    present_fields.frequency = (uint16_t) 2437;
	// 2 GHz spectrum
    present_fields.channel = (uint16_t) 0x0080;
    //TODO set RSSI properly
    present_fields.signal = (uint8_t) rssi;

	skb_pull(skb, data_offset);
	skb_push(skb, sizeof(ieee80211_radiotap_header_t) + sizeof(nexmon_radiotap_present_fields_t));
	memcpy(skb->data, &radiotaph, sizeof(ieee80211_radiotap_header_t));
    memcpy(skb->data + sizeof(ieee80211_radiotap_header_t), &present_fields, sizeof(nexmon_radiotap_present_fields_t));

	return skb;
}
