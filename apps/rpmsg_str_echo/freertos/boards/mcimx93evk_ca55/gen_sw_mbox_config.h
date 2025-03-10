/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GEN_SW_MBOX_CONFIG_H_
#define GEN_SW_MBOX_CONFIG_H_

#ifndef RTOSID
#define RTOSID 0
#endif

//! @def GEN_SW_MBOX_BASE
//!
//! Specify the sharing memory base address used for mailbox.
#define GEN_SW_MBOX_BASE (0xfe000000 + 0x1000 * RTOSID)
//@}

//! @def GEN_SW_MBOX_IRQ
//!
//! Specify the interrupt number for handling the received message.
#define GEN_SW_MBOX_IRQ (201)
//@}

//! @def GEN_SW_MBOX_REMOTE_IRQ
//!
//! Specify the interrupt number to trigger for notifying remote when send message.
#define GEN_SW_MBOX_REMOTE_IRQ (200)
//@}

//! @def GEN_SW_MBOX_IRQ_PRIO
//!
//! Specify the priority of the mailbox interrupt.
#define GEN_SW_MBOX_IRQ_PRIO (portLOWEST_USABLE_INTERRUPT_PRIORITY - 1)

#endif
