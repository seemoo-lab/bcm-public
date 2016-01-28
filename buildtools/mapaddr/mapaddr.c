#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <byteswap.h>

struct map {
	struct map* next;
	unsigned int addr;
	char *name;
};

int
main(void)
{
	FILE *fp_map, *fp_trace;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	struct map * start = NULL;
	struct map * current = NULL;
	unsigned int i = 0;
	char * afteraddr = " ";
	unsigned int j = 0;
	char * last = NULL;
	unsigned int lastaddr = 0;

	fp_map = fopen("firmware.map", "r");
	if (fp_map == NULL)
		exit(EXIT_FAILURE);

	fp_trace = fopen("trace.txt", "r");
	if (fp_trace == NULL)
		exit(EXIT_FAILURE);

	/* Read each line from the map file and create an entry in the linked list with address and name */
	while ((read = getline(&line, &len, fp_map)) != -1) {
		if(*(line+5) != ':')
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

	while ((read = getline(&line, &len, fp_trace)) != -1) {
		j = __bswap_32(strtoul(line, NULL, 16));
		for (current = start; current; current = current->next) {
			if(j < 0x800) {
				break;
			}
			if(j == current->addr) {
				printf("%08x= %s (%08x)\n", j, current->name, current->addr);
				break;
			}
			if(j < current->addr) {
				printf("%08x: %s (%08x+%d)\n", j, last, lastaddr, j - lastaddr);
				break;
			}
			last = current->name;
			lastaddr = current->addr;
		}
	}

	fclose(fp_map);
	fclose(fp_trace);
	if (line)
		free(line);
	exit(EXIT_SUCCESS);
}
