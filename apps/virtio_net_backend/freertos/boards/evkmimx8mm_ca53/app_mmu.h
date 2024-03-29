/*
 * Copyright 2022-2023 NXP
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
			      0xb8400000, KB(4),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("GEN_MBOX",				\
			      0xb8500000, KB(4),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY",					\
			      0x40000000, GB(0x1),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY1",				\
			      0x80000000, MB(0x13c),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY2",				\
			      0x94c00000, MB(0x238),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY3",				\
			      0xb8501000, KB(0x3ffc),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MEMORY4",				\
			      0xb8600000, MB(0x7a),			\
			      MT_NORMAL | MT_P_RW_U_RW | MT_NS),

#endif /* _APP_MMU_H_ */
