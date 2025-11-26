/*
 * Copyright 2022-2025 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/console/console.h>
#include <string.h>

#include "os/assert.h"

#include "log/log.h"
#include "rt_latency.h"

#define STACK_SIZE 4096
#define TEST_DURATION_FOREVER 0


K_THREAD_STACK_DEFINE(counter_stack, STACK_SIZE);

K_THREAD_STACK_DEFINE(cpu_load_stack, STACK_SIZE);

K_THREAD_STACK_DEFINE(cache_inval_stack, STACK_SIZE);

#ifndef SILENT_TESTING
K_THREAD_STACK_DEFINE(print_stack, STACK_SIZE);
#endif

#define MAX_TC_THREADS	8

static struct main_ctx{
	int test_case_id;
	bool started;
	struct rt_latency_ctx rt_ctx;
	struct ctrl_ctx ctrl;
	struct k_thread tc_thread[MAX_TC_THREADS];
	int threads_running_count;
} main_ctx;


static void counter_latency_test(void *p1, void *p2, void *p3)
{
	struct rt_latency_ctx *ctx = p1;
	const struct device *dev = (const struct device *)ctx->dev;
	int ret;

	k_object_access_grant(dev, k_current_get());

	do {
		ret = rt_latency_test(ctx);
		if (ret)
		{
			log_err("test failed!\n");
			k_thread_suspend(k_current_get());
		}
	} while (!ret);

	k_usleep(2000);
	k_thread_abort(k_current_get());
}

static void cpu_load_func(void *p1, void *p2, void *p3)
{
	struct rt_latency_ctx *ctx = p1;

	log_info("running cpu load %s\r\n",
		ctx->tc_load & RT_LATENCY_WITH_CPU_LOAD_SEM ? " (with extra semaphore load)" : "");

	do {
		cpu_load(ctx);
	} while(1);
}

static void cache_inval_func(void *p1, void *p2, void *p3)
{
	log_info("running cache invalidation\r\n");

	do {
		cache_inval();
	} while(1);
}

static void print_stats_func(void *p1, void *p2, void *p3)
{
	struct rt_latency_ctx *ctx = p1;

	do {
		k_msleep(STATS_PERIOD_SEC * 1000);

		print_stats(ctx);
	} while(1);
}

int start_test_case(void *context, int test_case_id)
{
	struct main_ctx *ctx = context;
	const struct device *counter_dev;
	const struct device *irq_load_dev;
	struct k_thread *benchmark_thread;
	struct k_thread *cpu_load_thread;
	struct k_thread *cache_invld_thread;
#ifndef SILENT_TESTING
	struct k_thread *print_thread;
#endif
	int ret;

	if (ctx->started)
		return -1;

	log_raw_info("\r\n\r\n");
	log_info("Running test case %d:\n", test_case_id);

	ctx->test_case_id = test_case_id;

	/* Initialize test case load conditions based on test case ID */
	ctx->rt_ctx.tc_load = rt_latency_get_tc_load(test_case_id);
	if (ctx->rt_ctx.tc_load < 0) {
		log_err("Wrong test conditions!\n");
		goto err;
	}

	/* Give required clocks some time to stabilize. In particular, nRF SoCs
	 * need such delay for the Xtal LF clock source to start and for this
	 * test to use the correct timing.
	 */
	k_busy_wait(USEC_PER_MSEC * 300);

	/* Create GPT threads with Highest Priority*/
	counter_dev = DEVICE_DT_GET(DT_ALIAS(counter0));
	if (!counter_dev) {
		log_err("Unable to get main counter device\n");
		goto err;
	}

	/* Use the second counter instance to create irq load with lower priority */
	irq_load_dev = DEVICE_DT_GET(DT_ALIAS(counter1));
	if (!irq_load_dev) {
		log_err("Unable to get IRQ load counter device\n");
		goto err;
	}

	/* Initialize test cases' context */
	ret = rt_latency_init((os_counter_t *)counter_dev, (os_counter_t *)irq_load_dev, &ctx->rt_ctx);
	if (ret != 0) {
		log_err("Initialization failed!\n");
		goto err;
	}

	benchmark_thread = &ctx->tc_thread[ctx->threads_running_count++];
	/* Benchmark task: main "high prio IRQ" task */
	k_thread_create(benchmark_thread, counter_stack, STACK_SIZE,
		counter_latency_test, &ctx->rt_ctx, NULL, NULL,
		K_HIGHEST_THREAD_PRIO, 0, K_FOREVER);

	k_busy_wait(USEC_PER_MSEC * 300);
#ifdef THREAD_CPU_BINDING
	k_thread_cpu_mask_clear(benchmark_thread);
	k_thread_cpu_mask_enable(benchmark_thread, GPT_CPU_BINDING);
#endif

#ifndef SILENT_TESTING
	/* Print Thread */
	print_thread = &ctx->tc_thread[ctx->threads_running_count++];
	k_thread_create(print_thread, print_stack, STACK_SIZE,
			print_stats_func, &ctx->rt_ctx, NULL, NULL,
			K_LOWEST_APPLICATION_THREAD_PRIO - 2, 0, K_FOREVER);
#ifdef THREAD_CPU_BINDING
	k_thread_cpu_mask_clear(print_thread);
	k_thread_cpu_mask_enable(print_thread, PRINT_CPU_BINDING);
#endif
	k_thread_start(print_thread);
#endif

	/* CPU Load Thread */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_CPU_LOAD) {
		cpu_load_thread = &ctx->tc_thread[ctx->threads_running_count++];
		k_thread_create(cpu_load_thread, cpu_load_stack, STACK_SIZE,
			cpu_load_func, &ctx->rt_ctx, NULL, NULL,
			K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_FOREVER);
#ifdef THREAD_CPU_BINDING
		k_thread_cpu_mask_clear(cpu_load_thread);
		k_thread_cpu_mask_enable(cpu_load_thread, CPU_LOAD_CPU_BINDING);
#endif
		k_thread_start(cpu_load_thread);
	}

	/* Cache Invalidate Thread */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_INVD_CACHE) {
		cache_invld_thread = &ctx->tc_thread[ctx->threads_running_count++];
		k_thread_create(cache_invld_thread, cache_inval_stack, STACK_SIZE,
			cache_inval_func, NULL, NULL, NULL,
			K_LOWEST_APPLICATION_THREAD_PRIO - 1, 0, K_FOREVER);
#ifdef THREAD_CPU_BINDING
		k_thread_cpu_mask_clear(cache_invld_thread);
		k_thread_cpu_mask_enable(cache_invld_thread, CPU_LOAD_CPU_BINDING);
#endif
		k_thread_start(cache_invld_thread);
	}

	/* Start GPT Threads */
	k_thread_start(benchmark_thread);

	ctx->started = true;

	return 0;
err:
	destroy_test_case(ctx);

	return -1;
}

void destroy_test_case(void *context)
{
	struct main_ctx *ctx = context;
	int i;

	if (!ctx->started)
		return;

	for (i = 0; i < ctx->threads_running_count; i++) {
		k_thread_abort(&ctx->tc_thread[i]);
	}

	ctx->threads_running_count = 0;

	rt_latency_destroy(&ctx->rt_ctx);

	ctx->started = false;
}

/* Get test duration in seconds from console input */
static int get_test_duration_from_console(void)
{
	char input_buffer[32] = {0};
	int test_duration_sec = 0;
	int i = 0;
	int ch;

	log_raw_info("\r\nEnter test duration ('0' for infinite loop):\r\n");
	log_raw_info("  - number for seconds (e.g., '10')\r\n");
	log_raw_info("  - number followed by 'h' for hours (e.g., '2h')\r\n");
	log_raw_info("  - number followed by 'd' for days (e.g., '1d')\r\n");
	log_raw_info("> ");

	/* Read characters until Enter key is pressed or buffer is full */
	while (i < sizeof(input_buffer) - 1) {
		ch = console_getchar();

		if (ch < 0) {
			/* No input available, retry */
			continue;
		}

		/* Enter key pressed (CR or LF) */
		if (ch == '\r' || ch == '\n') {
			log_raw_info("\r\n");
			break;
		}

		/* Handle backspace */
		if (ch == '\b' || ch == 127) {
			if (i > 0) {
				i--;
				input_buffer[i] = 0;
				log_raw_info("\b \b"); /* Erase character from terminal */
			}
			continue;
		}

		/* Accept digits and h/d unit specifiers */
		if ((ch >= '0' && ch <= '9') || (i > 0 && (ch == 'h' || ch == 'd'))) {
			input_buffer[i++] = (char)ch;
			log_raw_info("%c", (char)ch); /* Echo character */
		}
	}

	input_buffer[i] = '\0';

	/* Convert string to appropriate duration in seconds */
	if (i > 0) {
		char unit = input_buffer[i-1];

		if (unit == 'h' || unit == 'd') {
			/* Replace unit char with null terminator for conversion */
			input_buffer[i-1] = '\0';
			int value = atoi(input_buffer);

			if (unit == 'h') {
				test_duration_sec = value * 3600; /* Convert hours to seconds */
				log_raw_info("Test duration: %d hours (%d seconds)\r\n", value, test_duration_sec);
			} else { /* unit == 'd' */
				test_duration_sec = value * 86400; /* Convert days to seconds */
				log_raw_info("Test duration: %d days (%d seconds)\r\n", value, test_duration_sec);
			}
		} else {
			/* No unit specified, interpret as seconds */
			test_duration_sec = atoi(input_buffer);
			if (test_duration_sec > 0) {
				log_raw_info("Test duration: %d seconds\r\n", test_duration_sec);
			}
		}
	}

	return test_duration_sec;
}

/* Run @test_case_id for @duration_sec seconds */
static void run_test_case(void *context, int test_case_id, uint32_t duration_sec)
{
	struct main_ctx *ctx = context;
	k_timeout_t timeout;

	timeout = (duration_sec == TEST_DURATION_FOREVER) ? K_FOREVER : K_SECONDS((uint64_t)duration_sec);
	start_test_case(ctx, test_case_id);
	k_sleep(timeout);
	destroy_test_case(ctx);
	log_info("Test case %d completed\n", test_case_id);
}

static int command_handler(void *ctx)
{
	int input;
	uint32_t cmd_id;
	int test_duration_sec;

	log_raw_info("\r\nTest Case ID (1-6): \r\n");
	log_raw_info("\t1: no extra load\r\n");
	log_raw_info("\t2: extra CPU load\r\n");
	log_raw_info("\t3: extra IRQ load\r\n");
	log_raw_info("\t4: extra CPU load + semaphore load\r\n");
	log_raw_info("\t5: extra CPU load + Linux load (not provided by the test case)\r\n");
	log_raw_info("\t6: extra CPU load + cache flush\r\n");

	do
	{
		log_raw_info("\r\nPlease input the test case ID (1-6), 'q' for quit: ");
		input = console_getchar();
		if (input < 0)
		{
			continue;
		}
		else
		{
			log_raw_info("%c", (char)input);
			if (input == 'q' || input == 'Q')
				return -1;
		}
	} while ((input != '1') && (input != '2') && (input != '3') &&
		 (input != '4') && (input != '5') && (input != '6'));

	cmd_id = atoi((char *)&input);

	if (cmd_id >= RT_LATENCY_TEST_CASE_MAX) {
		log_err("Wrong test case id!\n");
		return -2;
	}

	/* Get test duration from user */
	test_duration_sec = get_test_duration_from_console();
	/* Given default duration */
	if (test_duration_sec <= 0) {
		test_duration_sec = TEST_DURATION_FOREVER;
		log_raw_info("Test duration: infinite loop\r\n");
	}

	run_test_case(ctx, cmd_id, (uint32_t)test_duration_sec);

	return 0;
}

int main(void)
{
	struct main_ctx *ctx = &main_ctx;

	memset(ctx, 0, sizeof(*ctx));

	console_init();

	log_raw_info("\r\n");
	log_raw_info("**************************************\r\n");
	log_raw_info(" Zephyr Latency Benchmark on Cortex-A\r\n");
	log_raw_info("**************************************\r\n");

	ctx->started = false;

	do {
		if ((!ctx->started) && (command_handler(ctx) == -1)) {
			break;
		}
	} while(1);

	log_raw_info("\r\n");
	log_info("Quit!\n");

	return 0;
}
