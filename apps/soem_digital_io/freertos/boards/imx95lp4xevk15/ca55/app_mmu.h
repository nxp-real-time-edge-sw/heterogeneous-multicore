/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_MMU_H_
#define _APP_MMU_H_

#define APP_MMU_ENTRIES		\
	MMU_REGION_FLAT_ENTRY("LPI2C2",						\
			      LPI2C2_BASE, KB(64),				\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),		\
	MMU_REGION_FLAT_ENTRY("NETC",						\
			      ENETC0_PSI_BASE, MB(2),				\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),		\
	MMU_REGION_FLAT_ENTRY("NETCPCI",						\
			      ENETC0_PCI_HDR_TYPE0_BASE, MB(2),				\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),		\
	MMU_REGION_FLAT_ENTRY("NETCBC",						\
			      BLK_CTRL_NETCMIX_BASE, KB(64),				\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),		\
	MMU_REGION_FLAT_ENTRY("TPM4",					\
			      TPM4_BASE, KB(64),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\
	MMU_REGION_FLAT_ENTRY("MSGINTR2",					\
			      MSGINTR2_BASE, KB(64),			\
			      MT_DEVICE_nGnRE | MT_P_RW_U_RW | MT_NS),	\

#endif /* _APP_MMU_H_ */
