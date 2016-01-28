#define _XOPEN_SOURCE 700
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

char *trace_array = NULL;
long trace_len = 0;

char *rom_array = NULL;
long rom_len = 0;

char *ram_array = NULL;
long ram_len = 0;

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
main(void)
{
	FILE *fp_map, *fp_trace;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *afteraddr = " ";

	fp_map = fopen("firmware.map", "r");
	if (fp_map == NULL)
		exit(EXIT_FAILURE);

	fp_trace = fopen("trace.txt", "r");
	if (fp_trace == NULL)
		exit(EXIT_FAILURE);

	read_file_to_array("trace.bin", &trace_array, &trace_len);
	read_file_to_array("../../firmware_patching/dma_txfast_path/rom.bin", &rom_array, &rom_len);
	read_file_to_array("../../bootimg_src/firmware/fw_bcmdhd.orig.bin", &ram_array, &ram_len);

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

	analyse_trace_file();

	fclose(fp_map);
	if (line)
		free(line);
	exit(EXIT_SUCCESS);
}
