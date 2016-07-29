#define _XOPEN_SOURCE 700
#define USE_LIBPCAP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>
#ifdef USE_LIBPCAP
#include <pcap.h>
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

struct phy_dump {
	int lr;
	unsigned short addr;
	unsigned short val;
} __attribute__((packed));

void
analyse_pcap_file(char *filename)
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];
	const struct phy_dump *packet;
	struct pcap_pkthdr header;
	char *fct_name;
	struct bpf_program fp;
	int i;

	char *trace_array = NULL;
	long trace_len = 0;

	if(!strcmp(filename, "any")) {
		pcap = pcap_open_live("any", BUFSIZ, 1, 0, errbuf);	
	} else {
		pcap = pcap_open_offline(filename, errbuf);	
	}

	// filter only UDPLITE frames
	pcap_compile(pcap, &fp, "proto 136", 0, 0);
	pcap_setfilter(pcap, &fp);

	if(!pcap) {
		printf("ERR: %s\n", errbuf);
		return;
	}

	while ((packet = pcap_next(pcap, &header)) != NULL) {
		if(packet[8].lr == 0xFFFFFFFF) {
			printf("\n\n");
		}
		
		for (i = 8; i < 128 + 8; i++) {
			printf("%08x %04x %04x\n", packet[i].lr, packet[i].addr, packet[i].val);
		}
	}
}

int
main(int argc, char **argv)
{
	analyse_pcap_file("any");

	exit(EXIT_SUCCESS);
}
