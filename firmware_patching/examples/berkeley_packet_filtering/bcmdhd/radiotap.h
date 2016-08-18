#ifndef RADIOTAP_H
#define RADIOTAP_H

#include <linux/types.h>

typedef struct ieee80211_radiotap_header {
    uint8_t it_version; /**< Version of the radiotap standard. Should be 0. */
    uint8_t it_pad; /**< Padding. Useless. */
    uint16_t it_len; /**< Length in bytes of the whole header. */
    uint32_t it_present; /**< A bitmap telling which fields are present. */
} ieee80211_radiotap_header_t;

struct nexmon_radiotap_present_fields {
    uint8_t flags;
    uint8_t padding1;
    uint16_t frequency;
    uint16_t channel;
    uint8_t signal;
} __attribute__((packed)); //stop the compiler from align our struct to 16 byte, we need it to be 15 bytes long

typedef struct nexmon_radiotap_present_fields nexmon_radiotap_present_fields_t;


#endif // RADIOTAP_H
