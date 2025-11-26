/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_BOARD_H_
#define _APP_BOARD_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Define device counter instances */
#define BOARD_COUNTER_0_BASE    TPM2
#define BOARD_COUNTER_0_IRQ     TPM2_IRQn
/* IRQ priority from 10 to 14,  lower values denote higher priorities */
#define BOARD_COUNTER_0_IRQ_PRIO    11
#define BOARD_COUNTER_1_BASE    TPM4
#define BOARD_COUNTER_1_IRQ     TPM4_IRQn
/* IRQ priority from 10 to 14,  lower values denote higher priorities */
#define BOARD_COUNTER_1_IRQ_PRIO    12

#endif /* _APP_BOARD_H_ */
