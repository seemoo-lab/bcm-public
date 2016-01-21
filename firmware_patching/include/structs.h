#ifndef STRUCTS_H
#define STRUCTS_H

#ifndef	PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif

struct osl_info {
	unsigned int pktalloced;
	int PAD[1];
	void *callback_when_dropped;
	unsigned int bustype;
} __attribute__((packed));

typedef struct sk_buff {
	struct sk_buff *next;
	struct sk_buff *prev;
	int PAD[2];
	void *data;
	short len;
} __attribute__((packed)) sk_buff;

struct tunables {
    char gap[62];
    short somebnd; // @ 0x38
    short rxbnd; // @ 0x40
};

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
} __attribute__((packed));

#endif /*STRUCTS_H */
