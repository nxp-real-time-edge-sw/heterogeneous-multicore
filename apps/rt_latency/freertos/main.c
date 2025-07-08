/*
 * Copyright 2021-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Freescale includes. */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "os/assert.h"
#include "os/counter.h"
#include "os/semaphore.h"

#include "stats/stats.h"
#include "log/log.h"

#include "rt_latency.h"
#include "rt_latency_board.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TEST_DURATION_FOREVER	0

#define NUM_OF_COUNTER		2

/* Task priorities. */
#define HIGHEST_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define LOWEST_TASK_PRIORITY  (1)

#define STACK_SIZE (configMINIMAL_STACK_SIZE + 100)
#define MAIN_STACK_SIZE	(STACK_SIZE + 1024)


/*******************************************************************************
 * Globals
 ******************************************************************************/

static struct main_ctx{
	bool started;

	struct rt_latency_ctx rt_ctx;
	struct ctrl_ctx ctrl;

	/* hard-coded number of elements ; only used to create/delete test case's
	* task handles, all at once */
	TaskHandle_t tc_taskHandles[8];
	int tasks_count;
} main_ctx;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Application tasks */
void cpu_load_task(void *pvParameters);
void cache_inval_task(void *pvParameters);
void log_task(void *pvParameters);
void benchmark_task(void *pvParameters);
int start_test_case(void *context, int test_case_id);
int command_handler(void *ctx);
void main_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
__WEAK void BOARD_InitClocks(void) {}

void cpu_load_task(void *pvParameters)
{
	struct rt_latency_ctx *ctx = pvParameters;

	log_info("running%s\n", ctx->tc_load & RT_LATENCY_WITH_CPU_LOAD_SEM ?
		       " (with extra semaphore load)" : "");

	do {
		cpu_load(ctx);
	} while(1);
}

void cache_inval_task(void *pvParameters)
{
	log_info("running\n");

	do {
		cache_inval();
	} while(1);
}

void log_task(void *pvParameters)
{
	struct rt_latency_ctx *ctx = pvParameters;

	do {
		vTaskDelay(pdMS_TO_TICKS(STATS_PERIOD_SEC * 1000));

		print_stats(ctx);
	} while(1);
}

void benchmark_task(void *pvParameters)
{
	int ret;
	struct rt_latency_ctx *ctx = pvParameters;

	log_info("running%s\n",
	       (ctx->tc_load & RT_LATENCY_WITH_IRQ_LOAD)  ? " (with IRQ load)" : "");

	do {
		ret = rt_latency_test(ctx);
		if (ret)
		{
			log_err("test failed!\n");
			vTaskSuspend(NULL);
		}
	} while (!ret);
}

int start_test_case(void *context, int test_case_id)
{
	struct main_ctx *ctx = context;
	os_counter_t *main_counter_dev;
	os_counter_t *irq_load_dev;
	BaseType_t xResult;

	if (ctx->started)
		return -1;

	log_raw_info("\r\n\r\n");
	log_info("Running test case %d:\n", test_case_id);

	main_counter_dev = GET_COUNTER_DEVICE_INSTANCE(0); //GPT1
	irq_load_dev = GET_COUNTER_DEVICE_INSTANCE(1); //GPT2

	/* Initialize test case load conditions based on test case ID */
	ctx->rt_ctx.tc_load = rt_latency_get_tc_load(test_case_id);
	if (ctx->rt_ctx.tc_load < 0) {
		log_err("Wrong test conditions!\n");
		goto err;
	}

	/* Initialize test cases' context */
	xResult = rt_latency_init(main_counter_dev, irq_load_dev, &ctx->rt_ctx);
	if (xResult) {
		log_err("Initialization failed!\n");
		goto err;
	}

	/* Benchmark task: main "high prio IRQ" task */
	xResult = xTaskCreate(benchmark_task, "benchmark_task", STACK_SIZE,
			       &ctx->rt_ctx, HIGHEST_TASK_PRIORITY - 1, &ctx->tc_taskHandles[ctx->tasks_count++]);
	if (xResult != pdPASS) {
		log_err("task creation failed!\n");
		goto err_task;
	}

	/* CPU Load task */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_CPU_LOAD) {
		xResult = xTaskCreate(cpu_load_task, "cpu_load_task", STACK_SIZE,
				       &ctx->rt_ctx, LOWEST_TASK_PRIORITY, &ctx->tc_taskHandles[ctx->tasks_count++]);
		if (xResult != pdPASS) {
			log_err("task creation failed!\n");
			goto err_task;
		}
	}

	/* Cache invalidate task */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_INVD_CACHE) {
		xResult = xTaskCreate(cache_inval_task, "cache_inval_task",
			       STACK_SIZE, NULL, LOWEST_TASK_PRIORITY + 1, &ctx->tc_taskHandles[ctx->tasks_count++]);
		if (xResult != pdPASS) {
			log_err("task creation failed!\n");
			goto err_task;
		}
	}

	/* Print task */
	xResult = xTaskCreate(log_task, "log_task", STACK_SIZE,
				&ctx->rt_ctx, LOWEST_TASK_PRIORITY + 2, &ctx->tc_taskHandles[ctx->tasks_count++]);
	if (xResult != pdPASS) {
		log_err("task creation failed!\n");
		goto err_task;
	}

	ctx->started = true;

	return 0;

err_task:
	destroy_test_case(ctx);
err:

	return -1;
}

void destroy_test_case(void *context)
{
	struct main_ctx *ctx = context;
	int i;

	if (!ctx->started)
		return;

	for (i = 0; i < ctx->tasks_count; i++) {
		vTaskDelete(ctx->tc_taskHandles[i]);
		ctx->tc_taskHandles[i] = NULL;
	}

	ctx->tasks_count = 0;

	rt_latency_destroy(&ctx->rt_ctx);

	ctx->started = false;
}

/* Get test duration in seconds from console input */
static int get_test_duration_from_console(void)
{
	char input_buffer[32] = {0};
	int test_duration_sec = 0;
	int i = 0;
	char ch;

	log_raw_info("\r\nEnter test duration ('0' for infinite loop):\r\n");
	log_raw_info("  - number for seconds (e.g., '10')\r\n");
	log_raw_info("  - number followed by 'h' for hours (e.g., '2h')\r\n");
	log_raw_info("  - number followed by 'd' for days (e.g., '1d')\r\n");
	log_raw_info("> ");

	/* Read characters until Enter key is pressed or buffer is full */
	while (i < sizeof(input_buffer) - 1) {
		if (DbgConsole_TryGetchar(&ch) != kStatus_Success) {
			/* No input available, retry */
			vTaskDelay(pdMS_TO_TICKS(200));
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
			input_buffer[i++] = ch;
			log_raw_info("%c", ch); /* Echo character */
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
	TickType_t timeout;

	timeout = (duration_sec == TEST_DURATION_FOREVER) ? portMAX_DELAY : pdMS_TO_TICKS(duration_sec * 1000);
	start_test_case(ctx, test_case_id);
	vTaskDelay(timeout);
	destroy_test_case(ctx);
	log_info("Test case %d completed\n", test_case_id);
}

int command_handler(void *ctx)
{
	char input = 0;
	uint32_t cmd_id;
	status_t status = (status_t)kStatus_Fail;
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
		log_raw_info("\rPlease input the test case ID (1-6), 'q' for quit: ");
		status = DbgConsole_TryGetchar(&input);
		if (status != kStatus_Success)
		{
			vTaskDelay(pdMS_TO_TICKS(500));
			continue;
		}
		else
		{
			log_raw_info("%c", input);
			if (input == 'q' || input == 'Q')
				return -1;
		}
	} while ((input != '1') && (input != '2') && (input != '3') &&
		 (input != '4') && (input != '5') && (input != '6'));

	cmd_id = atoi(&input);

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

void main_task(void *pvParameters)
{
	struct main_ctx *ctx = pvParameters;

	log_raw_info("\r\n");
	log_raw_info("****************************************\r\n");
	log_raw_info(" FreeRTOS Latency Benchmark on Cortex-A\r\n");
	log_raw_info("****************************************\r\n");

	ctx->started = false;

	do {
		if ((!ctx->started) && (command_handler(ctx) == -1)) {
			break;
		}
	} while(1);

	log_raw_info("\r\n");
	log_info("Quit!\n");

	/* dry up the log buffer */
	vTaskDelay(pdMS_TO_TICKS(100));
}

/*!
 * @brief Main function
 */
int main(void)
{
	BaseType_t xResult;

	rt_latency_board_init();

	/* Test cases scheduler task */
	xResult = xTaskCreate(main_task, "main_task", MAIN_STACK_SIZE,
			&main_ctx, LOWEST_TASK_PRIORITY, NULL);
	assert(xResult == pdPASS);

	/* Start scheduler */
	vTaskStartScheduler();

	for (;;)
		;

	return xResult;
}

