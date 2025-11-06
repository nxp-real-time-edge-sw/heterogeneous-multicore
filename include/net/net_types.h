/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NET_TYPES_H_
#define _NET_TYPES_H_

#define ETHERTYPE_IPV4	0x0800
#define ETHERTYPE_MSRP	0x22ea
#define ETHERTYPE_AVTP	0x22f0
#define ETHERTYPE_VLAN	0x8100
#define ETHERTYPE_IPV6	0x86DD
#define ETHERTYPE_MVRP	0x88f5
#define ETHERTYPE_MMRP	0x88f6
#define ETHERTYPE_PTP	0x88f7
#define ETHERTYPE_HSR	0x892f
#define ETHERTYPE_HSR_SUPERVISION 0x88fb

#ifndef ntohs

#if defined(__BIG_ENDIAN__)

#define ntohs(x) (x)
#define ntohl(x) (x)
#define htons(x) (x)
#define htonl(x) (x)

#else

#define ntohs(x) ((((x) & 0xFF00) >> 8) | (((x) & 0xFF) << 8))
#define ntohl(x) ((((x) & 0xFF000000) >> 24) | (((x) & 0xFF) << 24) | \
		(((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8))
#define htons(x) ((((x) & 0xFF00) >> 8) | (((x) & 0xFF) << 8))
#define htonl(x) ((((x) & 0xFF000000) >> 24) | (((x) & 0xFF) << 24) | \
		(((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8))

#endif /* __BIG_ENDIAN__ */

#endif /* ntohs */


#if defined(__BIG_ENDIAN__)

#define ntohll(x) (x)
#define htonll(x) (x)

#else

#define htonll(x) (((avb_u64)htonl((avb_u32)(x)))<<32 | htonl((avb_u32)((avb_u64)(x)>>32)))
#define ntohll(x) (((avb_u64)ntohl((avb_u32)(x)))<<32 | ntohl((avb_u32)((avb_u64)(x)>>32)))

#endif /* __BIG_ENDIAN__ */

#endif /* _NET_TYPES_H_ */
