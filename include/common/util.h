/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#define min_t(type, a, b)			\
	({ type t_a = (a);			\
	   type t_b = (b);			\
	   (void) (&t_a == &t_b);		\
	   t_a < t_b ? t_a : t_b; })
#define max_t(type, a, b)			\
	({ type t_a = (a);			\
	   type t_b = (b);			\
	   (void) (&t_a == &t_b);		\
	   t_a > t_b ? t_a : t_b; })

#define min(a, b)	min_t(typeof(a), a, b)
#define max(a, b)	max_t(typeof(a), a, b)

#endif
