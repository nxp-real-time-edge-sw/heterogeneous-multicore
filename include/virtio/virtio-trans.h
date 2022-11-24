/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TRANS_VIRTIO_H
#define _TRANS_VIRTIO_H

int virtio_trans_init(void *mmio_base);
int virtio_trans_exit(void);

#endif /* _TRANS_VIRTIO_H */
