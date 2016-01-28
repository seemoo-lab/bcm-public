#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>
#include "darm/darm.h"

#define ROM_SIZE (640*1024)
#define RAM_SIZE (768*1024)
#define RAM_START (0x180000)

struct map {
	struct map *next;
	unsigned int addr;
	char *name;
};


struct map *start = NULL;
struct map *current = NULL;

char *trace_file_array;
char *rom_array;
char *ram_array;

char *
read_file_to_array(char *filename)
{
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(filename, "rb");
	fseek(fileptr, 0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = (char *) malloc(filelen + 1);
	fread(buffer, filelen, 1, fileptr);
	fclose(fileptr);

	return buffer;
}

void
analyse_trace_file(FILE *fp_trace)
{
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	unsigned int i = 0, j = 0, k = 0, l = 0;
	char * last = NULL;
	unsigned int lastaddr = 0;
	char * indent[] = {"", " ", "  ", "   "};

	darm_t d; darm_str_t str;

	trace_file_array = read_file_to_array("trace.bin");
	rom_array = read_file_to_array("../../firmware_patching/dma_txfast_path/rom.bin");
	ram_array = read_file_to_array("../../bootimg_src/firmware/fw_bcmdhd.orig.bin");

	darm_init(&d);
	if(darm_disasm(&d, 0xf000, 0xff96, 1) != 0 && darm_str2(&d, &str, 1) == 0) {
		printf("-> %s %d\n", str.total, d.instr == I_BL);
		darm_dump(&d);
	} else {
		printf("error\n");
	}

	while ((read = getline(&line, &len, fp_trace)) != -1) {
		l = k;
		k = strtoul(line, NULL, 16);
		for (i = 0; i < 4; i++) {
			switch(i) {
				case 0:
					j = __bswap_32(k) - 5;
					break;
				case 1:
					j = __bswap_32(((l & 0xFF) << 24) | (k >> 8)) - 5;
					break;
				case 2:
					j = __bswap_32(((l & 0xFFFF) << 16) | (k >> 16)) - 5;
					break;
				case 3:
					j = __bswap_32(((l & 0xFFFFFF) << 8) | (k >> 24)) - 5;
					break;
			}

			for (current = start; current; current = current->next) {
				/* in th first part are no meanigful funtions, also not outside of ROM or RAM. As we are in thumb code, the target instruction address needs to have the last bit set. */
				if(j < 0x800 || (j > ROM_SIZE && j < RAM_START) || j > RAM_START + RAM_SIZE || j % 2) {
					break;
				}
				if(j == current->addr) {
					printf("%s%08x= %s (%08x)\n", indent[i], j, current->name, current->addr);
					break;
				}
				if(j < current->addr) {
					printf("%s%08x: %s (%08x+%d)\n", indent[i], j, last, lastaddr, j - lastaddr);
					break;
				}
				last = current->name;
				lastaddr = current->addr;
			}
		}
	}
}

int
main(void)
{
	FILE *fp_map, *fp_trace;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	unsigned int i = 0;
	char * afteraddr = " ";

	fp_map = fopen("firmware.map", "r");
	if (fp_map == NULL)
		exit(EXIT_FAILURE);

	fp_trace = fopen("trace.txt", "r");
	if (fp_trace == NULL)
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

	/* Output each of the read lines */
//	for (current = start; current; current = current->next) {
//		printf("%08x: %s", current->addr, current->name);
//	}

	analyse_trace_file(fp_trace);

	fclose(fp_map);
	fclose(fp_trace);
	if (line)
		free(line);
	exit(EXIT_SUCCESS);
}
