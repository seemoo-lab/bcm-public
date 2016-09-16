#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <byteswap.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <stdbool.h>
#include <errno.h>

#define WLC_IOCTL_MAGIC     0x14e46c77
#define DHD_IOCTL_MAGIC     0x00444944
#define WLC_GET_MAGIC                0
#define DHD_GET_MAGIC                0

#define WLC_GET_PROMISC             9
#define WLC_SET_PROMISC             10

#define WLC_GET_MONITOR             107
#define WLC_SET_MONITOR            108

/* Linux network driver ioctl encoding */
typedef struct nex_ioctl {
    uint cmd;   /* common ioctl definition */
    void *buf;  /* pointer to user buffer */
    uint len;   /* length of user buffer */
    bool set;   /* get or set request (optional) */
    uint used;  /* bytes read or written (optional) */
    uint needed;    /* bytes needed (optional) */
    uint driver;    /* to identify target driver */
} nex_ioctl_t;

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
    {"get-monitor", 'n', 0, 0, "Get monitor mode"},
    {"get-promisc", 'q', 0, 0, "Get promiscuous mode"},
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

        case 'n':
            get_monitor = 1;
            break;

        case 'q':
            get_promisc = 1;
            break;
        
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };

static int __nex_driver_io(struct ifreq *ifr, nex_ioctl_t *ioc)
{
    int s;
    int ret = 0;

    /* pass ioctl data */
    ifr->ifr_data = (caddr_t)ioc;

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("error: socket\n");

    ret = ioctl(s, SIOCDEVPRIVATE, ifr);
    if (ret < 0 && errno != EAGAIN)
        printf("%s: error\n", __FUNCTION__);

    /* cleanup */
    close(s);
    return ret;
}

/* This function is called by ioctl_setinformation_fe or ioctl_queryinformation_fe
 * for executing  remote commands or local commands
 */
static int
nex_ioctl(struct ifreq *ifr, int cmd, void *buf, int len, bool set)
{
    nex_ioctl_t ioc;
    int ret = 0;

    /* By default try to execute wl commands */
    int driver_magic = WLC_IOCTL_MAGIC;
    int get_magic = WLC_GET_MAGIC;

    // For local dhd commands execute dhd
    //driver_magic = DHD_IOCTL_MAGIC;
    //get_magic = DHD_GET_MAGIC;

    /* do it */
    ioc.cmd = cmd;
    ioc.buf = buf;
    ioc.len = len;
    ioc.set = set;
    ioc.driver = driver_magic;

    ret = __nex_driver_io(ifr, &ioc);
    if (ret < 0 && cmd != get_magic)
        ret = -1;
    return ret;
}

int
main(int argc, char **argv)
{
    struct ifreq ifr;
    int ret;
    int buf = 0;

    argp_parse(&argp, argc, argv, 0, 0, 0);

    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, "wlan0", 5);

    if(set_monitor) {
        buf = set_monitor_value;
        ret = nex_ioctl(&ifr, WLC_SET_MONITOR, &buf, 4, true);
        printf("ret: %d\n", ret);
    }

    if(set_promisc) {
        buf = set_promisc_value;
        ret = nex_ioctl(&ifr, WLC_SET_PROMISC, &buf, 4, true);
        printf("ret: %d\n", ret);
    }

    if(get_monitor) {
        ret = nex_ioctl(&ifr, WLC_GET_MONITOR, &buf, 4, false);
        printf("ret: %d\n", ret);
        printf("monitor: %d\n", buf);
    }

    if(get_promisc) {
        ret = nex_ioctl(&ifr, WLC_GET_PROMISC, &buf, 4, false);
        printf("ret: %d\n", ret);
        printf("promisc: %d\n", buf);
    }

    return 0;
}
