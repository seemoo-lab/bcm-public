#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct sk_buff {
    struct sk_buff *next;
    struct sk_buff *prev;
} sk_buff;

struct wlc_hw_info {
    struct wlc_info *wlc;
    int gap[4];
    void *di[6]; // only 4 byte
    int gap2[52];
    int maccontrol; // @ 0xe7
    int gap3[18];
    sk_buff *some_skbuff_ptr; // @ 0x134
};

struct wlc_info {
    struct wlc_pub *pub;
};

struct wlc_pub {
    int gap1[10];
    struct tunables *tunables; // @ 0x28
    char gap2[187];
    char is_amsdu; // @ 0xe7
}__attribute__((packed));

struct tunables {
    char gap[62];
    short somebnd; // @ 0x38
    short rxbnd; // @ 0x40
};


#endif /*STRUCTS_H */
