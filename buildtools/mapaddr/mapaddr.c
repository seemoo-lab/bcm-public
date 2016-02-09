#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>
#include "darm/darm.h"
#include <pcap.h>

#define ROM_SIZE (640*1024)
#define RAM_SIZE (768*1024)
#define RAM_START (0x180000)

struct map {
	struct map *next;
	unsigned int addr;
	char *name;
};

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


struct map *start = NULL;
struct map *current = NULL;

char *trace_array = NULL;
long trace_len = 0;

char *rom_array = NULL;
long rom_len = 0;

char *ram_array = NULL;
long ram_len = 0;

unsigned int my_trace[] = {
0x001e9854, 0x000043ae, 0x00180573, 0x001e9854 
, 0x000014e4, 0x000043ae, 0x00000000, 0x001f235d 
, 0x00000000, 0x001e983c, 0x18001000, 0x00000000 
, 0x001e9854, 0x0023f6a0, 0x001d9ad8, 0x0023f854 
, 0x00000005, 0x001d9b88, 0x000043ae, 0x001f2305 
, 0x000000a1, 0x00000025, 0x00000004, 0x00000025 
, 0x00000001, 0x001ec127, 0x00000812, 0x00000000 
, 0x001df8c8, 0x001d9ad8, 0x00000003, 0x001ed727 
, 0x0009eaa4, 0x001d3a34, 0x0023f71c, 0x00000001 
, 0x00000025, 0x00000004, 0x000000a1, 0x00000003 
, 0x000000a1, 0x00000003, 0x000200c5, 0x0023f71c 
, 0x099dbf08, 0x099dbf08, 0x39333334, 0x001d2900 
, 0x18002000, 0x00183887, 0x001d9acc, 0x00180000 
, 0x00240000, 0x18002000, 0x18102000, 0xb6feb9fb 
};

int
read_file_to_array(char *filename, char **buffer, long *filelen)
{
	FILE *fileptr;

	fileptr = fopen(filename, "rb");
	fseek(fileptr, 0, SEEK_END);
	*filelen = ftell(fileptr);
	rewind(fileptr);

	*buffer = (char *) malloc(*filelen + 1);
	fread(*buffer, *filelen, 1, fileptr);
	fclose(fileptr);

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
analyse_trace_file()
{
	long i = 0;
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
					printf("%08x= %s (%08x) ", j, fct_name, fct_addr);
					if (d.instr == I_BL) {
						get_name(v + d.imm, &bl_target, NULL);
						printf("%s %s\n", darm_mnemonic_name(d.instr), bl_target);
					} else {
						printf("%s r%d\n", darm_mnemonic_name(d.instr), d.Rm);
					}
				}
				break;
			case 2: // Somewhere in function
				disasm(&d, j, 1);
				if (d.instr == I_BL || d.instr == I_BLX) {
					printf("%08x: %s (%08x+%d) ", j, fct_name, fct_addr, j - fct_addr);
					if (d.instr == I_BL) {
						get_name(v + d.imm, &bl_target, NULL);
						printf("%s %s\n", darm_mnemonic_name(d.instr), bl_target);
					} else {
						printf("%s r%d\n", darm_mnemonic_name(d.instr), d.Rm);
					}
				}
				break;
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

	fp_map = fopen(filename, "r");
	if (fp_map == NULL)
		exit(EXIT_FAILURE);

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
	}

	fclose(fp_map);

	return 0;
}

int
main(void)
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];
	const unsigned char *packet;
	struct pcap_pkthdr header;
	char *fct_name;

	read_file_to_array("trace.bin", &trace_array, &trace_len);
	read_file_to_array("../../firmware_patching/dma_txfast_path/rom.bin", &rom_array, &rom_len);
	read_file_to_array("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", &ram_array, &ram_len);

	read_map_file("firmware.map");

	trace_array = (char *) my_trace;
	analyse_trace_file();

	pcap = pcap_open_offline("capture.pcap", errbuf);
	while ((packet = pcap_next(pcap, &header)) != NULL) {
		if(!memcmp(packet, ethernet_ipv6_udp_header_array, 18) && !memcmp(packet + 20, ethernet_ipv6_udp_header_array + 20, 42)) {
			get_name(*((unsigned int *) (packet+62)), &fct_name, NULL);
			printf("\n\n%s %08x %08x %08x %08x\n", fct_name, *((unsigned int *) (packet+62)), *((unsigned int *) (packet+66)), *((unsigned int *) (packet+70)), *((unsigned int *) (packet+74)));
			trace_array = packet + 60 + 4 * 4;
			//analyse_trace_file();
		}
	}

	//memcpy(trace_array, {0x0, 0x1, 0x2, 0x3}, 4);

	exit(EXIT_SUCCESS);
}
