#ifndef STRUCTS_H
#define STRUCTS_H

struct wlc_hw_info {
    struct wlc_info *wlc;
    int gap[4];
    void *di[6];
    int gap2[52];
    int maccontrol;
};

struct wlc_info {
    struct wlc_pub *pub;
};

struct wlc_pub {
    int gap[10];
    struct tunables *tunables;
};

struct tunables {
    int gap[16];
    int rxbnd;
};

typedef struct sk_buff {
    struct sk_buff *next;
    struct sk_buff *prev;
} sk_buff;

#endif /*STRUCTS_H */
