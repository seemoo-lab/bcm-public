#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <byteswap.h>
#include <pcap.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned char set_monitor = 0;
unsigned char set_monitor_value = 0;
unsigned char get_monitor = 0;
unsigned char set_promisc = 0;
unsigned char set_promisc_value = 0;
unsigned char get_promisc = 0;

const char *argp_program_version = "nexutil";
const char *argp_program_bug_address = "<mschulz@seemoo.tu-darmstadt.de>";

static char doc[] = "nexutil -- a program to control a nexmon firmware for broadcom chips.";

static struct argp_option options[] = {
	{"set-monitor", 'm', "BOOL", 0, "Set monitor mode"},
	{"set-promisc", 'p', "BOOL", 0, "Set promiscuous mode"},
	{ 0 }
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 'm':
			set_monitor = 1;
			set_monitor_value = strncmp(arg, "true", 4) ? 0 : 1;
			break;

		case 'p':
			set_promisc = 1;
			set_promisc_value = strncmp(arg, "true", 4) ? 0 : 1;
			break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };

// minimum length is 14 bytes
unsigned char frame[] = {
	'N', 'E', 'X', 'M', 'O', 'N', 'N', 'E', 'X', 'M', 'O', 'N', 0xFF, 0xFF, 0x00, 0x00
};

#define NEXMON_CTRL_SET_MONITOR 0
#define NEXMON_CTRL_GET_MONITOR 1
#define NEXMON_CTRL_SET_PROMISC 2
#define NEXMON_CTRL_GET_PROMISC 3

struct nexmon_ctrl_frame {
	unsigned char cmd;
	unsigned char value;
} __attribute__((packed));

void
send_to_chip()
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct pcap_pkthdr header;
	struct bpf_program fp;
	int iftype = 0;

	struct nexmon_ctrl_frame *ctrl_frame = (struct nexmon_ctrl_frame *) &frame[14];

	pcap = pcap_open_live("wlan0", BUFSIZ, 1, 0, errbuf);

	if(!pcap) {
		printf("ERR: %s\n", errbuf);
		return;
	}

	iftype = pcap_datalink(pcap);
	switch (iftype) {
		case 1:		// DLT_EN10MB; LINKTYPE_ETHERNET=1 
			//pcap_compile(pcap, &fp, "ether host ", 0, 0);
            break;
		case 127:	// DLT_IEEE802_11_RADIO; LINKTYPE_IEEE802_11_RADIO=127 
			//pcap_compile(pcap, &fp, "proto 136", 0, 0);
            break;
		default:
			printf("interface type not supported\n");
			exit(1);
            break;
	}
	//pcap_setfilter(pcap, &fp);

	if (set_monitor) {
		ctrl_frame->cmd = NEXMON_CTRL_SET_MONITOR;
		ctrl_frame->value = set_monitor_value;
		if (pcap_inject(pcap, &frame, sizeof(frame)) == -1) {
			pcap_perror(pcap,0);
			pcap_close(pcap);
			exit(1);
		}
	}

	if (get_monitor) {
		ctrl_frame->cmd = NEXMON_CTRL_GET_MONITOR;
		ctrl_frame->value = 0;
		if (pcap_inject(pcap, &frame, sizeof(frame)) == -1) {
			pcap_perror(pcap,0);
			pcap_close(pcap);
			exit(1);
		}
	}

	if (set_promisc) {
		ctrl_frame->cmd = NEXMON_CTRL_SET_PROMISC;
		ctrl_frame->value = set_promisc_value;
		if (pcap_inject(pcap, &frame, sizeof(frame)) == -1) {
			pcap_perror(pcap,0);
			pcap_close(pcap);
			exit(1);
		}
	}

	if (get_promisc) {
		ctrl_frame->cmd = NEXMON_CTRL_GET_PROMISC;
		ctrl_frame->value = 0;
		if (pcap_inject(pcap, &frame, sizeof(frame)) == -1) {
			pcap_perror(pcap,0);
			pcap_close(pcap);
			exit(1);
		}
	}
}

int
main(int argc, char **argv)
{
	argp_parse(&argp, argc, argv, 0, 0, 0);

	send_to_chip();

	exit(EXIT_SUCCESS);
}
