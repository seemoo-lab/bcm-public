#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <pcap.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

void
hexdump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != 0)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

int main(int argc, char *argv[]) {
    pcap_t *handle;
    char dev[] = "wlan0";
    char errbuf[PCAP_ERRBUF_SIZE];

    char frame[] = 
    {
        0x00, 0x00, 0x24, 0x00, 0x2f, 0x40, 0x00, 0xa0, /* Radiotap */
        0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x96, 0x5c, 0xd7, 0x2e, 0x00, 0x00, 0x00, 0x00, 
        0x10, 0x02, 0x6c, 0x09, 0xa0, 0x00, 0xca, 0x00,
        0x00, 0x00, 0xca, 0x00,
        0x80, 0x00, 0x00, 0x00,                         /* 802.11: type management */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 			/* destination address */
        0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,				/* source address */
        0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 			/* BSS address */
        0x10, 0x00, 									/* sequence and fragment numbers */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/* timestamp */
        0x64, 0x00, 0x21, 0x05, 
        0x00, 											/* tag number: SSID parameter set */
        0x06, 											/* tag length */
        'N', 'E', 'X', 'M', 'O', 'N',		 			/* SSID */
    };

    char deauth_frame[] = 
    {
        0x00, 0x00, 0x0c, 0x00, 0x04, 0x80, 0x00, 0x00, 0x02, 0x00, 0x18, 0x00, /* Radiotap */
        0xc0, 0x00, 0x3a, 0x01, 0x00, 0xf6, 0x63, 0x96, 0x6a, 0xa0, 0x24, 0x77, 0x03, 0xdd, 0xfe, 0x34, /* Deauth */
        0x00, 0xf6, 0x63, 0x96, 0x6a, 0xa0, 0xf0, 0x00
    };

    //Listen on interface
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 1;
    } else {
        printf("Pcap listen on device: %s\n", dev);
    }

    //Inject packet
    int ret = pcap_inject(handle, &frame, sizeof(frame));
    if (ret == -1) {
        fprintf(stderr, "Error on injecting packet!\n");
        pcap_perror(handle, 0);
        pcap_close(handle);
        return 1;
    } else {
        printf("Successful injected %d bytes\n", ret);
    }

    pcap_close(handle);
    
    return 0;
}
