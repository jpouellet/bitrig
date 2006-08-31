/*-
 * Copyright (c) 2004 Fujitsu Laboratories of America, Inc.
 * Copyright (c) 2004 Brian Fundakowski Feldman
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* Sizes, delays, etc. */
#define	PFF_TX_LIST_CNT			32
#define	PFF_RX_LIST_CNT			8
#define	PFF_FRAG_SIZE			1536	/* overkill for mgmt frames */
#define	PFF_DIRECT_MEMORY_OFFSET	0x1000
#define	PFF_DIRECT_MEMORY_SIZE		0x1000
#define	PFF_FIRMWARE_INTERNAL_OFFSET	0x20000
#define	PFF_WRITEIO_DELAY		10
#define	PFF_RESET_DELAY			50000

/* Registers */
#define PFF_REG_DEV_INT			0x0000	/* device interrupt */
#define	PFF_DEV_INT_RESET		0x00000001
#define	PFF_DEV_INT_UPDATE		0x00000002
#define	PFF_DEV_INT_WAKEUP		0x00000008
#define	PFF_DEV_INT_SLEEP		0x00000010
#define	PFF_REG_INT_STAT		0x0010	/* interrupt status */
#define	PFF_INT_STAT_UPDATE		0x00000002
#define	PFF_INT_STAT_INIT		0x00000004
#define	PFF_INT_STAT_WAKEUP		0x00000008
#define	PFF_INT_STAT_SLEEP		0x00000010
#define	PFF_INT_STAT_UNKNOWN0		0x00004000
#define	PFF_INT_STAT_UNKNOWN1		0x80000000
#define	PFF_INT_STAT_SOURCES		0x8000401e
#define	PFF_REG_INT_ACK			0x0014	/* interrupt acknowledgement */
#define	PFF_REG_INT_EN			0x0018	/* interrupt enable */
#define	PFF_REG_CTRL_BLK_BASE		0x0020	/* control block address */
#define	PFF_REG_GEN_PURP_COM		0x0024
#define	PFF_REG_DIR_MEM_BASE		0x0030
#define	PFF_REG_CTRL_STAT		0x0078
#define	PFF_CTRL_STAT_SLEEPMODE		0x00000200
#define	PFF_CTRL_STAT_CLOCKRUN		0x00800000
#define	PFF_CTRL_STAT_RESET		0x10000000
#define	PFF_CTRL_STAT_RAMBOOT		0x20000000
#define	PFF_CTRL_STAT_STARTHALTED	0x40000000
#define	PFF_CTRL_STAT_HOST_OVERRIDE	0x80000000

/*
 * The control block consists of a set of queues for low- and high-priority
 * data, and management, transmission and reception queues.  There is a
 * set of indices that gives the index (modulo queue size) of the current
 * progress in each.  Nearly all configuration is done from the management
 * queue interface.  Almost every structure is little-endian.
 */ 
enum pgt_queue {
	PFF_QUEUE_DATA_LOW_RX =		0,
	PFF_QUEUE_DATA_LOW_TX =		1,
	PFF_QUEUE_DATA_HIGH_RX =	2,
	PFF_QUEUE_DATA_HIGH_TX =	3,
	PFF_QUEUE_MGMT_RX =		4,
	PFF_QUEUE_MGMT_TX =		5
};

#define	PFF_QUEUE_COUNT			6
#define	PFF_QUEUE_DATA_RX_SIZE		8
#define	PFF_QUEUE_DATA_TX_SIZE		32
#define	PFF_QUEUE_MGMT_SIZE		4
#define	PFF_QUEUE_FULL_THRESHOLD	8

struct pgt_frag {
	uint32_t pf_addr;		/* physical host address */
	uint16_t pf_size;
	uint16_t pf_flags;
#define	PF_FLAG_MF	0x0001		/* more frags (frame continues) */
};

struct pgt_control_block {
	uint32_t	pcb_driver_curfrag[PFF_QUEUE_COUNT];
	uint32_t	pcb_device_curfrag[PFF_QUEUE_COUNT];
	struct pgt_frag	pcb_data_low_rx[PFF_QUEUE_DATA_RX_SIZE];
	struct pgt_frag	pcb_data_low_tx[PFF_QUEUE_DATA_TX_SIZE];
	struct pgt_frag	pcb_data_high_rx[PFF_QUEUE_DATA_RX_SIZE];
	struct pgt_frag	pcb_data_high_tx[PFF_QUEUE_DATA_TX_SIZE];
	struct pgt_frag	pcb_mgmt_rx[PFF_QUEUE_MGMT_SIZE];
	struct pgt_frag	pcb_mgmt_tx[PFF_QUEUE_MGMT_SIZE];
	uint32_t	pcb_padding;
};

/*
 * Unlike the rest of the structures, this is big-endian by default.
 * The Linux driver defines a PIMFOR_ETHERTYPE as 0x8828 (why?)
 */
enum pgt_mgmt_operation {
	PMF_OP_GET =		0,
	PMF_OP_SET =		1,
	PMF_OP_RESPONSE =	2,
	PMF_OP_ERROR =		3,
	PMF_OP_TRAP =		4
	/* may be more */
};

struct pgt_mgmt_frame {
	uint8_t		pmf_version;
#define	PMF_VER		0x01
	uint8_t		pmf_operation;
	uint32_t	pmf_oid;
	uint8_t		pmf_device;
#define PMF_DEV		0x00
	uint8_t		pmf_flags;
#define	PMF_FLAG_APP	0x01		/* application origin (?) */
#define	PMF_FLAG_LE	0x02		/* little-endian */
#define	PMF_FLAG_VALID	(PMF_FLAG_APP | PMF_FLAG_LE)
	uint32_t	pmf_size;
	/* 		data[];			*/
} __attribute__((packed));

struct pgt_data_frame {
	uint16_t	pdf_unknown;	/* seems to give status incl. crypto? */
	/*		data[];			*/
};

struct pgt_rx_annex {
	uint8_t 		pra_ether_dhost[ETHER_ADDR_LEN];
	uint8_t			pra_ether_shost[ETHER_ADDR_LEN];
	uint16_t		pra_unknown0;	/* always 0x0000 */
	uint16_t		pra_length;	/* always 0x1400 */
	uint32_t		pra_clock;	/* 1MHz timestamp */
	uint8_t			pra_flags;
#define	PRA_FLAG_BAD		0x01
	uint8_t			pra_unknown1;
	uint8_t			pra_rate;
	uint8_t			pra_unknown2;
	uint16_t		pra_frequency;
	uint16_t		pra_unknown3;
	uint8_t			pra_rssi;
	uint8_t			pra_pad[3];
	uint16_t		pra_ether_type;
} __attribute__((packed));

/*
 * OIDs used to communicate management information.
 */
enum pgt_oid {
	PFF_OID_MAC_ADDRESS =				0x00000000,
	/* uint8_t ether[6]; */
	PFF_OID_LINK_STATE = 				0x00000001,
	/* uint32_t rate; (500kbps units) */
	PFF_OID_BSS_TYPE =				0x10000000,
	/* uint32_t network; */
#define	PFF_BSS_TYPE_NONE			0
#define	PFF_BSS_TYPE_STA			1
#define	PFF_BSS_TYPE_IBSS			2
#define	PFF_BSS_TYPE_ANY			3
	PFF_OID_BSSID =					0x10000001,
	/* uint8_t bssid[6]; */
	PFF_OID_SSID =					0x10000002,
	/* struct pgt_obj_ssid; */
	PFF_OID_COUNTRY =				0x10000005,
	/* uint32_t country; guessing until I see some foreign hardware... */
#define	PFF_COUNTRY_USA				0
	PFF_OID_SSID_OVERRIDE =				0x10000006,
	/* struct pgt_obj_ssid; */
	PFF_OID_AUTH_MODE =				0x12000000,
	/* uint32_t auth; */
#define	PFF_AUTH_MODE_NONE			0
#define	PFF_AUTH_MODE_OPEN			1
#define	PFF_AUTH_MODE_SHARED			2
#define	PFF_AUTH_MODE_BOTH			3
	PFF_OID_PRIVACY_INVOKED =			0x12000001,
	/* uint32_t privacy; */
	PFF_OID_EXCLUDE_UNENCRYPTED =			0x12000002,
	/* uint32_t exunencrypted; */
	PFF_OID_DEFAULT_KEYNUM =			0x12000003,
	/* uint32_t defkey; */
	PFF_OID_DEFAULT_KEY0 =				0x12000004,
	/* struct pgt_obj_key; */
	PFF_OID_DEFAULT_KEY1 =				0x12000005,
	/* struct pgt_obj_key; */
	PFF_OID_DEFAULT_KEY2 =				0x12000006,
	/* struct pgt_obj_key; */
	PFF_OID_DEFAULT_KEY3 =				0x12000007,
	/* struct pgt_obj_key; */
	PFF_OID_STA_KEY =				0x12000008,
	PFF_OID_PSM =					0x14000000,
	/* uint32_t powersave; */
	PFF_OID_EAPAUTHSTA =				0x150007de,
	/* uint8_t sta[6]; */
	PFF_OID_EAPUNAUTHSTA =				0x150007df,
	/* uint8_t sta[6]; */
	PFF_OID_DOT1X =					0x150007e0,
	/* uint32_t dot1x; */
#define	PFF_DOT1X_AUTH_NONE			0
#define	PFF_DOT1X_AUTH_ENABLED			1
#define	PFF_DOT1X_KEYTX_ENABLED			2
	PFF_OID_SLOT_TIME =				0x17000000,
	/* uint32_t slottime; */
	PFF_OID_CHANNEL =				0x17000007,
	/* uint32_t channel; */
	PFF_OID_PREAMBLE_MODE =				0x17000009,
	/* uint32_t preamble; */
#define	PFF_OID_PREAMBLE_MODE_LONG		0
#define	PFF_OID_PREAMBLE_MODE_SHORT		1
#define	PFF_OID_PREAMBLE_MODE_DYNAMIC		2
	PFF_OID_RATES =	 				0x1700000a,
	/* uint8_t rates[]; nul terminated */
	PFF_OID_RSSI_VECTOR =				0x1700000d,
	PFF_OID_OUTPUT_POWER_TABLE =			0x1700000e,
	PFF_OID_OUTPUT_POWER =				0x1700000f,
	PFF_OID_SUPPORTED_RATES =	 		0x17000010,
	/* uint8_t rates[]; nul terminated */
	PFF_OID_NOISE_FLOOR =	 			0x17000013,
	/* uint32_t noise; */
	PFF_OID_SLOT_MODE =				0x17000017,
	/* uint32_t slot; */
#define	PFF_OID_SLOT_MODE_LONG			0
#define	PFF_OID_SLOT_MODE_SHORT			1
#define	PFF_OID_SLOT_MODE_DYNAMIC		2
	PFF_OID_EXTENDED_RATES =			0x17000020,
	/* uint8_t rates[]; nul terminated */
	PFF_OID_FREQUENCY =				0x17000011,
	/* uint32_t frequency; */
	PFF_OID_SUPPORTED_FREQUENCIES = 		0x17000012,
	/* struct pgt_obj_freq; */
	PFF_OID_PROFILE =				0x17000019,
	/* uint32_t profile; */
#define	PFF_PROFILE_B_ONLY			0
#define	PFF_PROFILE_MIXED_G_WIFI		1
#define	PFF_PROFILE_MIXED_LONG			2
#define	PFF_PROFILE_G_ONLY			3
#define	PFF_PROFILE_TEST			4
#define	PFF_PROFILE_B_WIFI			5
#define	PFF_PROFILE_A_ONLY			6
#define	PFF_PROFILE_MIXED_SHORT			7
	PFF_OID_DEAUTHENTICATE =			0x18000000,
	/* struct pgt_obj_mlme; */
	PFF_OID_AUTHENTICATE =				0x18000001,
	/* struct pgt_obj_mlme; */
	PFF_OID_DISASSOCIATE =				0x18000002,
	/* struct pgt_obj_mlme; */
	PFF_OID_ASSOCIATE =				0x18000003,
	/* struct pgt_obj_mlme; */
	PFF_OID_SCAN =					0x18000004,
	PFF_OID_BEACON =				0x18000005,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_PROBE =					0x18000006,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_DEAUTHENTICATEEX =			0x18000007,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_AUTHENTICATEEX =			0x18000008,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_DISASSOCIATEEX =			0x18000009,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_ASSOCIATEEX =				0x1800000a,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_REASSOCIATE =				0x1800000b,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_REASSOCIATEEX =				0x1800000c,
	/* struct pgt_obj_mlmeex; */
	PFF_OID_MLME_AUTO_LEVEL =			0x19000001,
	/* uint32_t mlme; */
#define	PFF_MLME_AUTO_LEVEL_AUTO		0
#define	PFF_MLME_AUTO_LEVEL_INTERMEDIATE	1
#define	PFF_MLME_AUTO_LEVEL_EXTENDED		2
	/* struct pgt_obj_buffer;*/
	PFF_OID_PSM_BUFFER =				0x19000004,
	/* struct pgt_obj_buffer;*/
#define	PFF_PSM_BUFFER_FRAME_COUNT		64
	PFF_OID_MAX_FRAME_BURST =			0x1b000008,
	/* uint32_t max_usec_grouped; */
	PFF_OID_BSS_FIND =				0x1c000042,
	/* struct pgt_obj_bss; */
	PFF_OID_BSS_LIST =				0x1c000043,
	/* struct pgt_obj_bsslist; */
	/* Initial settings. */
	PFF_OID_MODE =					0xff020003,
	/* uint32_t mode; */
#define	PFF_MODE_PROMISCUOUS			0
#define	PFF_MODE_CLIENT				1
#define	PFF_MODE_AP				2
#define	PFF_MODE_SNIFFER			3
	PFF_OID_CONFIG =				0xff020008,
	/* uint32_t flags; */
#define	PFF_CONFIG_MANUAL_RUN			0x00000001
#define	PFF_CONFIG_FRAME_TRAP			0x00000002
#define	PFF_CONFIG_RX_ANNEX			0x00000004
#define	PFF_CONFIG_TX_ANNEX			0x00000008
#define	PFF_CONFIG_WDS				0x00000010
	PFF_OID_PHY =					0xff02000d,
#define	PFF_OID_PHY_2400MHZ			0x00000001
#define	PFF_OID_PHY_5000MHZ			0x00000002
#define	PFF_OID_PHY_FAA				0x80000000
};

/*
 * Structures used to communicate via the OIDs.
 */
struct pgt_obj_ssid {
	uint8_t			pos_length;
	char			pos_ssid[33];
} __attribute__((packed));

struct pgt_obj_key {
	uint8_t			pok_type;
#define	PFF_OBJ_KEY_TYPE_WEP	0
#define	PFF_OBJ_KEY_TYPE_TKIP	1
	uint8_t			pok_length;
	uint8_t			pok_key[32];
} __attribute__((packed));

/*
 * Each mlme received includes the current 802.11 state.
 */
#define	PFF_MLME_STATE_NONE	0
#define	PFF_MLME_STATE_AUTHING	1
#define	PFF_MLME_STATE_AUTH	2
#define	PFF_MLME_STATE_ASSOCING	3

#define	PFF_MLME_STATE_ASSOC	5
#define	PFF_MLME_STATE_IBSS	6
#define	PFF_MLME_STATE_WDS	7

struct pgt_obj_mlme {
	uint8_t			pom_address[6];
	uint16_t		pom_id;		/* 802.11 association number */
	uint16_t		pom_state;
	uint16_t		pom_code;	/* 802.11 reason code */
} __attribute__((packed));

struct pgt_obj_mlmeex {
	uint8_t			pom_address[6];
	uint16_t		pom_id;
	uint16_t		pom_state;
	uint16_t		pom_code;
	uint16_t		pom_size;
	uint8_t			pom_data[0];
} __attribute__((packed));

struct pgt_obj_buffer {
	uint32_t		pob_size;
	uint32_t		pob_addr;
} __attribute__((packed));

struct pgt_obj_bss {
	uint8_t			pob_address[6];
	uint16_t		pob_padding0;
	uint8_t			pob_state;
	uint8_t			pob_reserved;
	uint16_t		pob_age;
	uint8_t			pob_quality;
	uint8_t			pob_rssi;
	struct pgt_obj_ssid	pob_ssid;
	uint16_t		pob_channel;
	uint8_t			pob_beacon_period;
	uint8_t			pob_dtim_period;
	uint16_t		pob_capinfo;
	uint16_t		pob_rates;
	uint16_t		pob_basic_rates;
	uint16_t		pob_padding1;
} __attribute__((packed));

struct pgt_obj_bsslist {
	uint32_t		pob_count;
	struct pgt_obj_bss	pob_bsslist[0];
#define	PFF_OBJ_BSSLIST_NBSS	24
} __attribute__((packed));

struct pgt_obj_frequencies {
	uint16_t		pof_count;
	uint16_t		pof_freqlist_mhz[0];
} __attribute__((packed));
