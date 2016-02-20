#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <byteswap.h>
#include "darm/darm.h"
#ifdef USE_LIBPCAP
#include <pcap.h>
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define ROM_SIZE (640*1024)
#define RAM_SIZE (768*1024)
#define RAM_START (0x180000)

#define AS_STR2(TEXT) #TEXT
#define AS_STR(TEXT) AS_STR2(TEXT)

struct map *start = NULL;
struct map *current = NULL;

char *rom_array = NULL;
long rom_len = 0;

char *ram_array = NULL;
long ram_len = 0;

#ifdef USE_LIBPCAP
char *pcap_file_name = NULL;
#endif
char *map_file_name = AS_STR(MAP_FILE_NAME);
char *rom_file_name = AS_STR(ROM_FILE_NAME);
char *ram_file_name = AS_STR(RAM_FILE_NAME);

short udp_port;
char udpbuf[USHRT_MAX];

int print_not_found = 0;

const char *argp_program_version = "mapaddr";
const char *argp_program_bug_address = "<mschulz@seemoo.tu-darmstadt.de>";

static char doc[] = "mapaddr -- a program to read stack dumps of the BCM4339 chip and print the stack trace.";

static struct argp_option options[] = {
#ifdef USE_LIBPCAP
	{"pcapfile", 'c', "FILE", 0, "Read pcap file from FILE"},
#endif
	{"mapfile", 'm', "FILE", 0, "Read symbol map from FILE instead of " AS_STR(MAP_FILE_NAME)},
	{"romfile", 'o', "FILE", 0, "Read rom from FILE instead of " AS_STR(ROM_FILE_NAME)},
	{"ramfile", 'a', "FILE", 0, "Read ram from FILE instead of " AS_STR(RAM_FILE_NAME)},
	{"udpport", 'u', "PORT", 0, "Listen for UDP frames containing stack traces on PORT"},
	{"print-not-found", 'n', "BOOL", 0, "Set true if non-branch target stack values should be printed"},
	{ 0 }
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key) {
#ifdef USE_LIBPCAP
		case 'c':
			pcap_file_name = arg;
			break;
#endif

		case 'm':
			map_file_name = arg;
			break;

		case 'o':
			rom_file_name = arg;
			break;

		case 'a':
			ram_file_name = arg;
			break;

		case 'u':
			udp_port = strtol(arg, NULL, 10);
			break;

		case 'n':
			print_not_found = strncmp(arg, "true", 4) ? 0 : 1;
			break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

struct map {
	struct map *next;
	unsigned int addr;
	char *name;
};

static struct argp argp = { options, parse_opt, 0, doc };

unsigned char ethernet_ipv6_udp_header_array[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   /* ETHERNET: Destination MAC Address */
  'N', 'E', 'X', 'M', 'O', 'N',         /* ETHERNET: Source MAC Address */
  0x86, 0xDD,                           /* ETHERNET: Type */
  0x60, 0x00, 0x00, 0x00,               /* IPv6: Version / Traffic Class / Flow Label */
  0x00, 0x08,                           /* IPv6: Payload Length */
  0x88,                                 /* IPv6: Next Header = UDPLite */
  0x01,                                 /* IPv6: Hop Limit */
  0xFF, 0x02, 0x00, 0x00,               /* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,               /* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,               /* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x01,               /* IPv6: Source IP */
  0xFF, 0x02, 0x00, 0x00,               /* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,               /* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,               /* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x01,               /* IPv6: Destination IP */
  0xD6, 0xD8,                           /* UDPLITE: Source Port = 55000 */
  0xD6, 0xD8,                           /* UDPLITE: Destination Port = 55000 */
  0x00, 0x08,                           /* UDPLITE: Checksum Coverage */
  0x52, 0x46,                           /* UDPLITE: Checksum only over UDPLITE header*/
};

int
read_file_to_array(char *filename, char **buffer, long *filelen)
{
	FILE *fileptr;

	if((fileptr = fopen(filename, "rb"))) {
		fseek(fileptr, 0, SEEK_END);
		*filelen = ftell(fileptr);
		rewind(fileptr);

		*buffer = (char *) malloc(*filelen + 1);
		fread(*buffer, *filelen, 1, fileptr);
		fclose(fileptr);

		return *filelen;
	}

	return 0;
}

int
get_words(unsigned int addr, unsigned short *low, unsigned short *high)
{
	if (addr < rom_len - 4) {
		*low = *((unsigned short *) (rom_array + addr));
		*high = *((unsigned short *) (rom_array + addr + 2));
		return 1;
	}

	if (addr > RAM_START && addr < RAM_START + ram_len - 4) {
		*low = *((unsigned short *) (ram_array + addr - RAM_START));
		*high = *((unsigned short *) (ram_array + addr - RAM_START + 2));
		return 2;
	}

	return 0;
}

int
get_name(unsigned int addr, char **sym_name, unsigned int *sym_addr)
{
	char *last = NULL;
	unsigned int lastaddr = 0;

	for (current = start; current; current = current->next) {
		/* in th first part are no meanigful funtions, also not outside of ROM or RAM. As we are in thumb code, the target instruction address needs to have the last bit set. */
		if(addr < 0x800 || (addr > ROM_SIZE && addr < RAM_START) || addr > RAM_START + RAM_SIZE) {
			return 0;
		}
		if(addr == current->addr) {
			if (sym_name) *sym_name = current->name;
			if (sym_addr) *sym_addr = current->addr;
			return 1;
		}
		if(addr < current->addr) {
			if (sym_name) *sym_name = last;
			if (sym_addr) *sym_addr = lastaddr;
			return 2;
		}
		last = current->name;
		lastaddr = current->addr;
	}

	return 0;
}

int
disasm(darm_t *d, unsigned int addr, int cont) {
	unsigned short low, high;
	int ret;

	get_words(addr, &low, &high);
	ret = darm_disasm(d, low, high, addr + 1);

	// If a thumb instruction was found, we continue with the next instruction
	if(cont && ret == 1) {
		get_words(addr + 2, &low, &high);
		return darm_disasm(d, low, high, addr + 1);
	} else {
		return ret;
	}
}

void
print_instr(unsigned int stack_value, darm_t *d, char *bl_target)
{
	if (d->instr == I_BL) {
		get_name(stack_value + d->imm, &bl_target, NULL);
		printf("%s %s\n", darm_mnemonic_name(d->instr), bl_target);
	} else {
		printf("%s r%d\n", darm_mnemonic_name(d->instr), d->Rm);
	}
}

void
analyse_trace(char *trace_array, unsigned int trace_len)
{
	unsigned int i = 0;
	unsigned int v, j;
	char *bl_target = NULL;
	char *fct_name = NULL;
	unsigned int fct_addr = 0;
	darm_t d;

	darm_init(&d);

	for(i = 0; i < trace_len - 4; i++) {
		v = *((unsigned int *) (trace_array+i));
		j = v - 5;

		switch(get_name(j, &fct_name, &fct_addr)) {
			case 0: // not found or ignored
				break;
			case 1: // Exact match
				disasm(&d, j, 1);
				if (d.instr == I_BL || d.instr == I_BLX) {
					printf("%04d %08x= %s (%08x) ", i, j, fct_name, fct_addr);
					print_instr(v, &d, bl_target);
					continue;
				}
				break;
			case 2: // Somewhere in function
				disasm(&d, j, 1);
				if (d.instr == I_BL || d.instr == I_BLX) {
					printf("%04d %08x: %s (%08x+%d) ", i, j, fct_name, fct_addr, j - fct_addr);
					print_instr(v, &d, bl_target);
					continue;
				}
				break;
		}

		switch(get_name(v - 1, &fct_name, &fct_addr)) {
			case 0:
				break;
			case 1:
				printf("%04d %08x= %s (%08x)\n", i, v - 1, fct_name, fct_addr);
				continue;
				break;
			case 2:
				break;
		}

		if ((i % 4 == 0) && print_not_found) {
			printf("%04d %08x\n", i, v);
		}
	}
}

int
read_map_file(char *filename)
{
	FILE *fp_map;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *afteraddr = " ";
	unsigned int count = 0;

	if((fp_map = fopen(filename, "r"))) {
		/* Read each line from the map file and create an entry in the linked list with address and name */
		while ((read = getline(&line, &len, fp_map)) != -1) {
			if(line[5] != ':' || !memcmp(line+21, "loc_", 4))
				continue;
			if(!start) {
				start = malloc(sizeof(struct map));
				current = start;
			} else {
				current->next = malloc(sizeof(struct map));
				current = current->next;
			}
			memset(current, 0, sizeof(struct map));
			current->addr = strtoul(line+6,&afteraddr,16);
			current->name = line+21;

			/* remove new line charecter at the end of the line */
			*(line+strlen(line)-2) = '\0';
			line = NULL;

			count++;
		}

		fclose(fp_map);

		return count;
	}

	return 0;
}

#ifdef USE_LIBPCAP
void
analyse_pcap_file(char *filename)
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];
	const unsigned char *packet;
	struct pcap_pkthdr header;
	char *fct_name;

	char *trace_array = NULL;
	long trace_len = 0;

	pcap = pcap_open_offline(filename, errbuf);
	while ((packet = pcap_next(pcap, &header)) != NULL) {
		if(!memcmp(packet, ethernet_ipv6_udp_header_array, 18) && !memcmp(packet + 20, ethernet_ipv6_udp_header_array + 20, 42)) {
			get_name(*((unsigned int *) (packet+62)), &fct_name, NULL);
			printf("\n\n%s %08x %08x %08x %08x\n", fct_name, *((unsigned int *) (packet+62)), *((unsigned int *) (packet+66)), *((unsigned int *) (packet+70)), *((unsigned int *) (packet+74)));
			trace_array = (char *) packet + 60 + 4 * 4;
			trace_len = header.len - 60 - 4 * 4;
			analyse_trace(trace_array, trace_len);
		}
	}
}
#endif

int
analyse_udp_frames(short port)
{
	struct sockaddr_in si_me, si_other;
	socklen_t slen = sizeof(si_other);
	int sock, l;

	if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		return -1;

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *) &si_me, sizeof(si_me))==-1)
		return -2;

	while(1) {
		if ((l = recvfrom(sock, udpbuf, sizeof(udpbuf), 0, (struct sockaddr *) &si_other, &slen)) == -1)
			return -3;
		printf("Received packet from %s:%d length: %d\n", 
		inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), slen);
		analyse_trace(udpbuf, l);
	}

	close(sock);
}

int
main(int argc, char **argv)
{
	argp_parse(&argp, argc, argv, 0, 0, 0);

//	read_file_to_array("../../firmware_patching/dma_txfast_path/rom.bin", &rom_array, &rom_len);
//	read_file_to_array("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", &ram_array, &ram_len);

	if(!read_file_to_array(rom_file_name, &rom_array, &rom_len)) {
		fprintf(stderr, "ERR: rom file empty or unavailable.\n");
		exit(EXIT_FAILURE);
	}

	if(!read_file_to_array(ram_file_name, &ram_array, &ram_len)) {
		fprintf(stderr, "ERR: ram file empty or unavailable.\n");
		exit(EXIT_FAILURE);
	}

	if(!read_map_file(map_file_name)) {
		fprintf(stderr, "ERR: map file empty or unavailable.\n");
		exit(EXIT_FAILURE);
	}

#ifdef USE_LIBPCAP
	if (pcap_file_name)
		analyse_pcap_file(pcap_file_name);
#else
	if (0);
#endif
	else if (udp_port)
		analyse_udp_frames(udp_port);
	else
		fprintf(stderr, "ERR: no source selected.\n");

	exit(EXIT_SUCCESS);
}
