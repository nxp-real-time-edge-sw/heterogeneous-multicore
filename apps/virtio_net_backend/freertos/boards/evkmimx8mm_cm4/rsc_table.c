/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 * Copyright 2020,2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This file provides the default empty resource table data structure
 * placed in the .resource_table section of the ELF file. This facilitates
 * basic support of remoteproc firmware loading from the Linux kernel.
 *
 * The .resource_table section has to be present in the ELF file in order
 * for the remoteproc ELF parser to accept the binary.
 *
 * See other multicore examples such as those utilizing rpmsg for a examples
 * of non-empty resource table.
 *
 */

#include <stdint.h>
#include <string.h>
#include "rsc_table.h"

/* Place resource table in special ELF section */
#if defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".resource_table")))
#elif defined(__ICCARM__)
#pragma location = ".resource_table"
#else
#error Compiler not supported!
#endif

const uint32_t resource_table[] = {
    /* Version */
    1,

    /* Number of table entries - resource table empty */
    0,

    /* reserved fields */
    0,
    0,
};

void copyResourceTable(void)
{
    /* TCM ECC is enabled in default. Must initialize to use. */
    memset((void *)RESOURCE_TABLE_START, 0, RESOURCE_TABLE_SIZE);
    memcpy((void *)RESOURCE_TABLE_START, &resource_table, sizeof(resource_table));
}
