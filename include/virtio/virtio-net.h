/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NET_VIRTIO_H
#define _NET_VIRTIO_H

int virtio_net_init(void *mmio_base, void *bridge);
int virtio_net_exit(void *mmio_base, void *bridge);

#endif /* _NET_VIRTIO_H */
