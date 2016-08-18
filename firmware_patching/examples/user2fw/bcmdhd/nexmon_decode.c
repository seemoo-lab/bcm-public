#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/types.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <osl.h>

#include "radiotap.h"
#include "nexmon.h"

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
    // offset before the 802.11 header starts
    skb_trim(skb, pkt_len);
    data_offset = 58;
    if (data[28] & 4) {
        data_offset += 2;
    }

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
