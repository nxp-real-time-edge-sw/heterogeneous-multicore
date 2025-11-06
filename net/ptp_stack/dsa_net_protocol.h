/*
* Copyright 2023-2024 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _DSA_NET_PROTOCOL_H_
#define _DSA_NET_PROTOCOL_H_

/* 802.1Q VLAN Tag:
 * +-----------+------+------+-----------------+
 * |    TPID   | PCP  | CFI  |     VID         |
 * +-----------+------+------+-----------------+
 * |    16bit  | 3bit | 1bit |    12bit        |
 * +-----------+------+------+-----------------+
 * TPID = 0x8100 for VLAN Tag
 *
 * Custom 12-bit VID field used in 802.1Q VLAN
 * +-----------+-----+-----------------+-----------+-----------------------+
 * | 11  | 10  |  9  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * +-----------+-----+-----------------+-----------+-----------------------+
 * |    RSV    | VBID|    SWITCH_ID    |   VBID    |          PORT         |
 * +-----------+-----+-----------------+-----------+-----------------------+
 *
 * RSV - VID[11:10]:
 *             Reserved. Must be set to 3 (0b11).
 *
 * SWITCH_ID - VID[8:6]:
 *             Index of switch within DSA tree. Unused.
 *
 * VBID - { VID[9], VID[5:4] }:
 *             Virtual bridge ID. Unused.
 *
 * PORT - VID[3:0]:
 *             Index of switch port.
 *
 * 802.1Q DSA Tag:
 * +-----------+-----------------+-----------------+-----------------+
 * |    TPID   |     DSA         |    Timestamp    |     ts_id       |
 * |           |                 |    (optional)   |  (optional)     |
 * +-----------+-----------------+-----------------+-----------------+
 * |    16bit  |    16bit        |     64bit * 2   |     32bit       |
 * +-----------+-----------------+-----------------+-----------------+
 *
 * TPID = 0xDADC for DSA Tag
 *
 * * Custom 16-bit DSA field for DSA frames from MCU Switch to MPU Host
 * +-----------+------+-----------+-------+-------+--------+-----------+---------+
 * |   15      |  14  |     13    |   12  |  11   | 10 - 9 |   7 - 4   |  3 - 0  |
 * +-----------+------+-----------+-------+-------+--------+-----------+---------+
 * |0 (to host)| META | HOST Only | RX TS | TX TS |        | Switch ID | Port ID |
 * +-----------+------+-----------+-------+-------+--------+-----------+---------+
 *
 * RX TS: the "Timestamp" field is the timestamp of this received frame.
 * TX TS: the "Timestamp" field is the timestamp retieved of the tranmitted frame,
 *        "ts_id" is followed with "Timestamp".
 * Switch ID: hardware switch ID that the frame received from.
 * Port ID: switch port ID that the frame received from.
 *
 * Custom 16-bit DSA field used in DSA frames from MPU host to MCU Switch
 *
 * +-----------+------+----------------+---------+--------+-----------+---------+
 * |   15      |  14  |   13   |   12  |  11     | 10 - 9 |  7 - 4    |  3 - 0  |
 * +-----------+------+--------+-------+---------+--------+-----------+---------+
 * | 1 (to sw )| META |        |       | TAKE TS |        | SWITCH ID | PORT ID |
 * +-----------+------+--------+-------+------  -+--------+-----------+---------+
 *
 * Take TS: need to retrieve timestamp and send it back to MPU host.
 * Switch ID: hardware switch ID that the frame transmitted to.
 * Port ID: switch port ID that the frame transmitted to.
 */

#define DSA_TAG_LENGTH              (4) /* in bytes includes TPID, DSA_TAG */
#define TIMESTAMP_LENGTH            (8)

#define VLAN_8021Q_TPID             (0x8100)
#define DEFAULT_CUSTOM_8021Q_VID    (0x0C00)
#define VLAN_VID_PORT_MASK          0xF
#define VLAN_TAG(port_id)           ((htons((DEFAULT_CUSTOM_8021Q_VID | ((port_id) & VLAN_VID_PORT_MASK))) << 16) | htons(VLAN_8021Q_TPID))
#define VLAN_TAG_LENGTH             (4) /* length in bytes includes includes TPID (2B) and VLAN TAG (2B) */

#define DSA_8021Q_TPID             (0xDADC)
#define DSA_TO_HOST_OFFSET			15
#define DSA_TO_HOST_META_OFFSET		14
#define DSA_TO_HOST_HOSTONLY_OFFSET	13
#define DSA_TO_HOST_RX_TS_OFFSET	12
#define DSA_TO_HOST_TX_TS_OFFSET	11
#define DSA_TO_HOST_SW_ID_OFFSET	4
#define DSA_TO_HOST_SW_ID_MASK		0xF
#define DSA_TO_HOST_PORT_OFFSET		0
#define DSA_TO_HOST_PORT_MASK		0xF
#define DSA_TO_HOST_RX_TS_DEFAULT_DSA	0x1000 /* TO_HOST:0, META:0, HOST Only:0, Rx TS:1, Tx TS:0, SW ID:0, Port ID: 0 */
#define DSA_TO_HOST_RX_TS_TAG(port_id)		((htons((DSA_TO_HOST_RX_TS_DEFAULT_DSA | \
											((port_id) & DSA_TO_HOST_PORT_MASK))) << 16) | \
											htons(DSA_8021Q_TPID))
#define DSA_TO_HOST_RX_TS_TAG_LEN	20  /* length in bytes includes TPID (2B), DSA field (2B), timestamp (16B) */

#define DSA_TO_HOST_TX_TS_DEFAULT_DSA	0x0800 /* TO_HOST:0, META:0, HOST Only:0, Rx TS:0, Tx TS:1, SW ID:0, Port ID: 0 */
#define DSA_TO_HOST_TX_TS_TAG(port_id)		((htons((DSA_TO_HOST_TX_TS_DEFAULT_DSA | \
											((port_id) & DSA_TO_HOST_PORT_MASK))) << 16) | \
											htons(DSA_8021Q_TPID))
#define DSA_TO_HOST_TX_TS_TAG_LEN	24  /* length in bytes includes TPID (2B), DSA field (2B), timestamp (16B), ts_id (4B) */


#define DSA_TO_SW_OFFSET			15
#define DSA_TO_SW_META_OFFSET		14
#define DSA_TO_SW_TAKE_TS_OFFSET	11
#define DSA_TO_SW_RX_TS_OFFSET		12
#define DSA_TO_SW_SW_ID_OFFSET		4
#define DSA_TO_SW_SW_ID_MASK		0xF
#define DSA_TO_SW_PORT_OFFSET		0
#define DSA_TO_SW_PORT_MASK			0xF

#define DSA_TO_SW_META(x)			((x) & (1 << DSA_TO_SW_META_OFFSET))
#define DSA_TO_SW_TAKE_TS(x)		((x) & (1 << DSA_TO_SW_TAKE_TS_OFFSET))
#define DSA_TO_SW_PORT_ID(x)		(((x) >> DSA_TO_SW_SW_ID_OFFSET) & DSA_TO_SW_SW_ID_MASK)

#define DS_TO_SW_NONTS_TAG_LEN		4  /* lenght in bytes includes TPID (2B), DSA field (2B) */
#define DS_TO_SW_TS_TAG_LEN			8  /* lenght in bytes includes TPID (2B), DSA field (2B), ts_id (4B) */

#define MAC_ADDR_TOTAL_LENGTH       (12)


struct eth_hdr {
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t ether_type;
	uint16_t dsa_custom;
	union {
		struct vlan_f {
			uint16_t eth_type;
		} vlan_frame;
		struct dsa_to_h_t_f {
			uint64_t ts;
			uint64_t cycles;
			uint32_t ts_id;
			uint16_t eth_type;
		} dsa_to_host_tx_ts_frame;
		struct dsa_to_h_r_f {
			uint64_t ts;
			uint64_t cycles;
			uint16_t eth_type;
		} dsa_to_host_rx_ts_frame;
		struct dsa_tp_s_f {
			uint32_t ts_id;
			uint16_t eth_type;
		} dsa_to_sw_ts_frame;
	} u;
};

#endif /* _DSA_NET_PROTOCOL_H_ */
