/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>

#include "fsl_ram_console.h"

static void help(const char *app_name)
{
	fprintf(stderr, "\nRAM Console tool to dump console log.\n"
		"Usage:%s -a buffer_address [-r refresh_period_in_seconds]\n", app_name);
	fprintf(stderr, "\t\t-a buffer_address:\tRAM Console buffer physical address\n");
	fprintf(stderr,	"\t\t-r refresh_period:\tRefresh period time in seconds for continuous dump, ctrl+c to exit\n\n");
}

static void print_buf(uint32_t start_pos, uint32_t end_pos, void *buf_addr, bool print_blink, bool new_line)
{
	uint32_t i;
	char ch;
	bool begin_dump;

	if (new_line)
		begin_dump = false;
	else
		begin_dump = true;

	for (i = start_pos; i < end_pos; i++) {
		ch = *((unsigned char *) (buf_addr + RAM_CONSOLE_HEAD_SIZE + i));
		if (!begin_dump) {
			if (ch == 0xa || ch == 0xd)
				begin_dump = true;
		} else
			printf("%c", ((isascii(ch)) && (isprint(ch) || ch == 0xa || ch == 0xd)) ? ch : '.');
	}

	if (print_blink) {
		printf("\033[5m%s\033[0m", " > ");
	}
}

int main(int argc, char **argv)
{
	struct ram_console_header *header;
	int fd;
	void *buf_addr;
	char ch;
	off_t phy_addr = 0;
	int loop_time = 0;
	size_t len;
	uint32_t pos, real_pos, last_pos = 0, real_last_pos;
	int i;
	int opt;

	while ((opt = getopt(argc,argv, "a:r:")) != -1) {
		switch(opt) {
		case 'a':
			phy_addr = strtoul(optarg, 0, 0);
			break;
		case 'r':
			loop_time = atoi(optarg);
			break;
		default:
			help(argv[0]);
			exit(1);
		}
	};

	if (phy_addr == 0) {
		help(argv[0]);
		exit(1);
	}

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		fprintf(stderr, "openning memory device failed\r\n");
		exit(1);
	}
    
	/* Map RAM Console Header firstly */
	header = mmap(NULL, RAM_CONSOLE_HEAD_SIZE, PROT_READ, MAP_SHARED, fd, phy_addr);
	if(header == (void *) -1) {
		fprintf(stderr, "mapping header memory failed\r\n");
		goto err;
	}

	if (strcmp(header->flag_string, RAM_CONSOLE_HEAD_STR) != 0) {
		fprintf(stderr, "Invalid RAM Console header at 0x%lx\r\n", phy_addr);
		goto err;
	}
	len = header->ram_console_buf_size;
	pos = header->pos;

	if(munmap(header, RAM_CONSOLE_HEAD_SIZE) == -1) {
		fprintf(stderr, "unmapping header memory failed\r\n");
		goto err;
	}
	/* Re-map RAM Console Header and Buffer Memory */
	buf_addr = mmap(NULL, len + RAM_CONSOLE_HEAD_SIZE, PROT_READ, MAP_SHARED, fd, phy_addr);
	if(buf_addr == (void *) -1) {
		fprintf(stderr, "mapping buffer memory failed\r\n");
		goto err;
	}

	real_pos = pos % len;
	printf("RAM Console@0x%lx:\r\n", phy_addr);
	if (pos > len) {
		print_buf(real_pos + 1, len, buf_addr, false, true);
	}
	print_buf(0, real_pos, buf_addr, true, false);
	fflush(stdout);

	if(munmap(buf_addr, len + RAM_CONSOLE_HEAD_SIZE) == -1) {
		fprintf(stderr, "unmapping buffer memory failed\r\n");
		goto err;
	}
	last_pos = pos;

	while (loop_time > 0) {
		sleep(loop_time);

		/* Re-map RAM Console Header and Buffer Memory */
		buf_addr = mmap(NULL, len + RAM_CONSOLE_HEAD_SIZE, PROT_READ, MAP_SHARED, fd, phy_addr);
		if(buf_addr == (void *) -1) {
			fprintf(stderr, "mapping buffer memory failed\r\n");
			goto err;
		}
		header = buf_addr;
		pos = header->pos;
		real_pos = pos %len;
		real_last_pos = last_pos %len;
		if (pos == last_pos) {
			continue;
		} else if (pos > last_pos) {
			if (pos - last_pos <= len) {
				/* return 3 blank space which is used by blink display */
				printf("\b\b\b");
				if (real_pos > real_last_pos) {
					print_buf(real_last_pos, real_pos, buf_addr, true, false);
				} else {
					print_buf(real_last_pos, len, buf_addr, false, false);
					print_buf(0, real_pos, buf_addr, true, false);
				}
			} else { /* dump whole buffer */
				print_buf(real_pos, len, buf_addr, false, true);
				print_buf(0, real_pos, buf_addr, true, false);
			}
		} else {
			/* Overflow, re-print whole buffer */
			print_buf(real_pos, len, buf_addr, false, true);
			print_buf(0, real_pos, buf_addr, true, false);
		}
		last_pos = pos;
		fflush(stdout);

		if(munmap(buf_addr, len + RAM_CONSOLE_HEAD_SIZE) == -1) {
			fprintf(stderr, "unmapping buffer memory failed\r\n");
			goto err;
		}
	};
	close(fd);
	exit(0);

err:
	close(fd);
	exit(1);
}
