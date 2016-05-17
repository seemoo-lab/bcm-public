#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <pcap.h>
#include "ethertype.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

struct sockaddr_nl src_addr, dest_addr;
int sock_fd;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;

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
    printf("filterutil\n");
    struct bpf_program code_compiled;

    char *errbuf[PCAP_ERRBUF_SIZE];
    char *filter_str;
    struct bpf_insn *insn;
    int i;
    uint32_t complete_len;

    if(argc != 2) {
        printf("Must provied _one_ argument!\n");
        return -1;
    } else {
        printf("Sending: \"%s\", len: %d\n", argv[1], strlen(argv[1]));
        filter_str = argv[1];
    }

    // Gen BPF Code from user input; optimization enabled
    if(pcap_compile_nopcap(65535, DLT_IEEE802_11_RADIO, &code_compiled, filter_str, 1, PCAP_NETMASK_UNKNOWN) == -1) {
        printf("ERROR: could not compile BPF filter: %s\n", filter_str);
        return -1;
    } else {
        printf("Generated %d filters\n", code_compiled.bf_len);
    }

    insn = code_compiled.bf_insns;

    for (i = 0; i < code_compiled.bf_len; ++insn, ++i) {
        printf("{ 0x%x, %d, %d, 0x%08x },\n", insn->code, insn->jt, insn->jf, insn->k);
    }

    hexdump(0, code_compiled.bf_insns, code_compiled.bf_len * sizeof(struct bpf_insn));

    // Open socket for netlink interface
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        printf("ERROR: Could not open socket!\n");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    //Check for max payload size
    if(code_compiled.bf_len * sizeof(struct bpf_insn) > NLMSG_SPACE(MAX_PAYLOAD)) {
        printf("ERROR: compiled filter is to long for netlink: %d", code_compiled.bf_len * sizeof(struct bpf_insn));
        return -1;
    }

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD) + sizeof(code_compiled.bf_len));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    //Copy length of filter array in front
    complete_len = code_compiled.bf_len * sizeof(struct bpf_insn);
    memcpy(NLMSG_DATA(nlh), &complete_len, sizeof(uint32_t));

    //Copy filter to netlink buffer; jump over length at the beginning
    memcpy(NLMSG_DATA(nlh) + sizeof(complete_len), code_compiled.bf_insns, code_compiled.bf_len * sizeof(struct bpf_insn));

    printf("DEBUG: SENDING filter data (incl length): \n");
    hexdump(0, NLMSG_DATA(nlh), complete_len + sizeof(uint32_t));

    printf("DEBUG: SENDING filter length: %u\n", *((unsigned int *) NLMSG_DATA(nlh)));

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf("Sending message to kernel...\n");
    if(sendmsg(sock_fd, &msg, 0) < 0) {
        printf("... failed!\n");
    } else {
        printf("... successful\n");
    }

    free(nlh);
    close(sock_fd);

    return 0;
}
