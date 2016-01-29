#ifndef STRUCTS_H
#define STRUCTS_H

#include "types.h"
#include "bcmdhd/bcmcdc.h"

#ifndef	PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif

struct osl_info {
	unsigned int pktalloced;
	int PAD[1];
	void *callback_when_dropped;
	unsigned int bustype;
} __attribute__((packed));

typedef struct sk_buff {
	struct sk_buff *next;
	struct sk_buff *prev;
	int PAD[2];
	void *data;
	short len;
} __attribute__((packed)) sk_buff;

struct tunables {
    char gap[62];
    short somebnd; // @ 0x38
    short rxbnd; // @ 0x40
};

struct wlc_hw_info {
    struct wlc_info *wlc;
    int gap[4];
    void *di[6]; // only 4 byte
    int gap2[52];
    int maccontrol; // @ 0xe7
    int gap3[18];
    sk_buff *some_skbuff_ptr; // @ 0x134
};

struct wlc_info {
    struct wlc_pub *pub;
};

struct wlc_pub {
    int gap1[10];
    struct tunables *tunables; // @ 0x28
    char gap2[187];
    char is_amsdu; // @ 0xe7
} __attribute__((packed));






struct ethernet_header {
    uint8 dst[6];
    uint8 src[6];
    uint16 type;
} __attribute__((packed));

struct ipv6_header {
    uint32 version_traffic_class_flow_label;
    uint16 payload_length;
    uint8 next_header;
    uint8 hop_limit;
    uint8 src_ip[16];
    uint8 dst_ip[16];
} __attribute__((packed));

struct ip_header {
    uint8 version_ihl;
    uint8 dscp_ecn;
    uint16 total_length;
    uint16 identification;
    uint16 flags_fragment_offset;
    uint8 ttl;
    uint8 protocol;
    uint16 header_checksum;
    union {
        uint32 integer;
        uint8 array[4];
    } src_ip;
    union {
        uint32 integer;
        uint8 array[4];
    } dst_ip;
} __attribute__((packed));

struct udp_header {
    uint16 src_port;
    uint16 dst_port;
    union {
        uint16 length;          /* UDP: length of UDP header and payload */
        uint16 checksum_coverage;   /* UDPLITE: checksum_coverage */
    } len_chk_cov;
    uint16 checksum;
} __attribute__((packed));

struct bdc_ethernet_ip_udp_header {
    struct bdc_header bdc;
    struct ethernet_header ethernet;
    struct ip_header ip;
    struct udp_header udp;
} __attribute__((packed));

struct bdc_ethernet_ipv6_udp_header {
    struct bdc_header bdc;
    struct ethernet_header ethernet;
    struct ipv6_header ipv6;
    struct udp_header udp;
    uint8 payload[1];
} __attribute__((packed));

struct nexmon_header {
    uint32 hooked_fct;
    uint32 args[3];
    uint8 payload[1];
} __attribute__((packed));

#endif /*STRUCTS_H */
