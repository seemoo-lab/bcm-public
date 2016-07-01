#pragma once

struct tsf {
	unsigned int tsf_l;
	unsigned int tsf_h;
} __attribute__((packed));

struct ieee80211_radiotap_header {
	char it_version;
	char it_pad;
	short it_len;
	int it_present;
	struct tsf tsf;
	char flags;
	char PAD;
	unsigned short chan_freq;
	unsigned short chan_flags;
	char dbm_antsignal;
	char PAD;
//	unsigned short vht_known;
//	unsigned char vht_flags;
//	unsigned char vht_bandwidth;
//	unsigned char vht_mcs_nss[4];
//	unsigned char vht_coding;
//	unsigned char vht_group_id;
//	unsigned short vht_partial_aid;
	unsigned char vendor_oui[3];
	unsigned char vendor_sub_namespace;
	unsigned short vendor_skip_length;
} __attribute__((packed));

#define IEEE80211_RADIOTAP_TSFT					(1 << 0)
#define IEEE80211_RADIOTAP_FLAGS				(1 << 1)
#define IEEE80211_RADIOTAP_RATE					(1 << 2)
#define IEEE80211_RADIOTAP_CHANNEL				(1 << 3)
#define IEEE80211_RADIOTAP_FHSS					(1 << 4)
#define IEEE80211_RADIOTAP_DBM_ANTSIGNAL		(1 << 5)
#define IEEE80211_RADIOTAP_DBM_ANTNOISE			(1 << 6)
#define IEEE80211_RADIOTAP_LOCK_QUALITY			(1 << 7)
#define IEEE80211_RADIOTAP_TX_ATTENUATION		(1 << 8)
#define IEEE80211_RADIOTAP_DB_TX_ATTENUATION	(1 << 9)
#define IEEE80211_RADIOTAP_DBM_TX_POWER			(1 << 10)
#define IEEE80211_RADIOTAP_ANTENNA				(1 << 11)
#define IEEE80211_RADIOTAP_DB_ANTSIGNAL			(1 << 12)
#define IEEE80211_RADIOTAP_DB_ANTNOISE			(1 << 13)
#define IEEE80211_RADIOTAP_RX_FLAGS				(1 << 14)
#define IEEE80211_RADIOTAP_TX_FLAGS				(1 << 15)
#define IEEE80211_RADIOTAP_RTS_RETRIES			(1 << 16)
#define IEEE80211_RADIOTAP_DATA_RETRIES			(1 << 17)

#define IEEE80211_RADIOTAP_MCS					(1 << 19)
#define IEEE80211_RADIOTAP_AMPDU_STATUS			(1 << 20)
#define IEEE80211_RADIOTAP_VHT					(1 << 21)

#define IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE	(1 << 29)
#define IEEE80211_RADIOTAP_VENDOR_NAMESPACE		(1 << 30)
#define IEEE80211_RADIOTAP_EXT					(1 << 31)

/* For IEEE80211_RADIOTAP_FLAGS */
#define	IEEE80211_RADIOTAP_F_CFP		0x01	/* sent/received during CFP */
#define	IEEE80211_RADIOTAP_F_SHORTPRE	0x02	/* sent/received with short preamble */
#define	IEEE80211_RADIOTAP_F_WEP		0x04	/* sent/received with WEP encryption */
#define	IEEE80211_RADIOTAP_F_FRAG		0x08	/* sent/received with fragmentation */
#define	IEEE80211_RADIOTAP_F_FCS		0x10	/* frame includes FCS */
#define	IEEE80211_RADIOTAP_F_DATAPAD	0x20	/* frame has padding between 802.11 header and payload (to 32-bit boundary) */
#define IEEE80211_RADIOTAP_F_BADFCS		0x40	/* bad FCS */

/* For IEEE80211_RADIOTAP_RX_FLAGS */
#define IEEE80211_RADIOTAP_F_RX_BADPLCP	0x0002	/* frame has bad PLCP */

/* For IEEE80211_RADIOTAP_TX_FLAGS */
#define IEEE80211_RADIOTAP_F_TX_FAIL	0x0001	/* failed due to excessive retries */
#define IEEE80211_RADIOTAP_F_TX_CTS		0x0002	/* used cts 'protection' */
#define IEEE80211_RADIOTAP_F_TX_RTS		0x0004	/* used rts/cts handshake */
#define IEEE80211_RADIOTAP_F_TX_NOACK	0x0008	/* don't expect an ack */

/* For IEEE80211_RADIOTAP_VHT */
#define IEEE80211_RADIOTAP_VHT_KNOWN_STBC           0x0001
#define IEEE80211_RADIOTAP_VHT_KNOWN_TXOP_PS_NA         0x0002
#define IEEE80211_RADIOTAP_VHT_KNOWN_GI             0x0004
#define IEEE80211_RADIOTAP_VHT_KNOWN_SGI_NSYM_DIS       0x0008
#define IEEE80211_RADIOTAP_VHT_KNOWN_LDPC_EXTRA_OFDM_SYM    0x0010
#define IEEE80211_RADIOTAP_VHT_KNOWN_BEAMFORMED         0x0020
#define IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH          0x0040
#define IEEE80211_RADIOTAP_VHT_KNOWN_GROUP_ID           0x0080
#define IEEE80211_RADIOTAP_VHT_KNOWN_PARTIAL_AID        0x0100

#define IEEE80211_RADIOTAP_VHT_FLAG_STBC            0x01
#define IEEE80211_RADIOTAP_VHT_FLAG_TXOP_PS_NA          0x02
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI             0x04
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI_NSYM_M10_9      0x08
#define IEEE80211_RADIOTAP_VHT_FLAG_LDPC_EXTRA_OFDM_SYM     0x10
#define IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED          0x20

