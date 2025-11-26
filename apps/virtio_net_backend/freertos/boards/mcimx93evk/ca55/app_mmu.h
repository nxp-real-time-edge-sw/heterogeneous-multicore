/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_MMU_H_
#define _APP_MMU_H_

#define APP_MMU_ENTRIES							\
	MMU_REGION_FLAT_ENTRY("ENET",					\
			      ENET_BASE, KB(64),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("VIRTIO",					\
			      0xfc700000, KB(4),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("GEN_MBOX",				\
			      0xfe000000, KB(4),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY",					\
			      0x80000000, MB(0x500),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY1",				\
			      0xd1000000, MB(0x2b7),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY2",				\
			      0xfc800000, MB(0x18),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY3",				\
			      0xfe001000, KB(0x3ffc),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY4",				\
			      0xff000000, MB(0x10),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),

#endif /* _APP_MMU_H_ */
