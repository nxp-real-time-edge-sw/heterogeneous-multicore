/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _IOVEC_H
#define _IOVEC_H

#include <stddef.h>
#include "common/util.h"

struct iovec {
	void *addr;
	size_t len;
};

static inline size_t iov_size(const struct iovec *iovec, size_t len)
{
	size_t size = 0, i;

	for (i = 0; i < len; i++)
		size += iovec[i].len;

	return size;
}

static inline int memcpy_toiovec(struct iovec *iov, uint8_t *data, int len)
{
	while (len > 0) {
		if (iov->len) {
			int copy = min_t(uint32_t, iov->len, len);
			memcpy(iov->addr, data, copy);
			data += copy;
			iov->addr += copy;
			len -= copy;
			iov->len -= copy;
		}
		iov++;
	}

	return 0;
}

static inline int memcpy_fromiovec(uint8_t *data, struct iovec *iov, int len)
{
	while (len > 0) {
		if (iov->len) {
			int copy = min_t(uint32_t, iov->len, len);
			memcpy(data, iov->addr, copy);
			data += copy;
			iov->addr += copy;
			len -= copy;
			iov->len -= copy;
		}
		iov++;
	}

	return 0;
}
#endif
