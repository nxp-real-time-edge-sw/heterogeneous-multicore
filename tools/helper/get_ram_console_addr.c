/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <rtos_memory.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	printf("0x%08x", RAM_CONSOLE_ADDR);

	return 0;
}
