/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>

#include "fsl_ram_console.h"

int main(int argc, char **argv)
{
	struct ram_console_header *header;
	int fd;
	void *buf_addr;
	char ch;
	off_t phy_addr;
	size_t len, pos;
	int i;
	
	if(argc != 2) {
		fprintf(stderr, "\nRAM Console Tool to dump Console log.\n"
			"Usage:\t\t%s address \n"
			"address:\tRAM Console memory physical address\n\n",
			argv[0]);
		exit(1);
	}
	/* Physical address */
	phy_addr = strtoul(argv[1], 0, 0);

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		fprintf(stderr, "openning memory device failed\r\n");
		exit(1);
	}
    
	/* Map RAM Console Header firstly */
	header = mmap(NULL, RAM_CONSOLE_HEAD_SIZE, PROT_READ, MAP_SHARED, fd, phy_addr);
	if(header == (void *) -1) {
		fprintf(stderr, "mapping header memory failed\r\n");
		exit(1);
	}

	if (strcmp(header->flag_string, RAM_CONSOLE_HEAD_STR) != 0) {
		fprintf(stderr, "Invalid RAM Console header at 0x%lx\r\n", phy_addr);
		exit(1);
	}
	len = header->ram_console_buf_size;
	pos = header->pos;

	if(munmap(header, RAM_CONSOLE_HEAD_SIZE) == -1) {
		fprintf(stderr, "unmapping header memory failed\r\n");
		exit(1);
	}
	/* Re-map RAM Console Header and Buffer Memory */
	buf_addr = mmap(NULL, len + RAM_CONSOLE_HEAD_SIZE, PROT_READ, MAP_SHARED, fd, phy_addr);
	if(buf_addr == (void *) -1) {
		fprintf(stderr, "mapping buffer memory failed\r\n");
		exit(1);
	}

	printf("RAM Console@0x%lx:\r\n", phy_addr);
	for (i = 0; i < len; i++) {
		/* blink '>' at the tail of the buffer */
		if ( i == pos)
			printf("\033[5m%s\033[0m", " > ");
		ch = *((unsigned char *) (buf_addr + RAM_CONSOLE_HEAD_SIZE + i));
		printf("%c", ((isascii(ch)) && (isprint(ch) || ch == 0xa || ch == 0xd)) ? ch : '.');
	}
	printf("\r\n");
	fflush(stdout);

	if(munmap(buf_addr, len + RAM_CONSOLE_HEAD_SIZE) == -1) {
		fprintf(stderr, "unmapping buffer memory failed\r\n");
		exit(1);
	}

	close(fd);

	return 0;
}
