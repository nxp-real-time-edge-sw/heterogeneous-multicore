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

#define NUM_OF_COUNTER		2

/* Task priorities. */
#define HIGHEST_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define LOWEST_TASK_PRIORITY  (0)

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
void command_handler(void *ctx);
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
	int hnd_idx = 0;
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
			       &ctx->rt_ctx, HIGHEST_TASK_PRIORITY - 1, &ctx->tc_taskHandles[hnd_idx++]);
	if (xResult != pdPASS) {
		log_err("task creation failed!\n");
		goto err_task;
	}

	/* CPU Load task */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_CPU_LOAD) {
		xResult = xTaskCreate(cpu_load_task, "cpu_load_task", STACK_SIZE,
				       &ctx->rt_ctx, LOWEST_TASK_PRIORITY, &ctx->tc_taskHandles[hnd_idx++]);
		if (xResult != pdPASS) {
			log_err("task creation failed!\n");
			goto err_task;
		}
	}

	/* Cache invalidate task */
	if (ctx->rt_ctx.tc_load & RT_LATENCY_WITH_INVD_CACHE) {
		xResult = xTaskCreate(cache_inval_task, "cache_inval_task",
			       STACK_SIZE, NULL, LOWEST_TASK_PRIORITY + 1, &ctx->tc_taskHandles[hnd_idx++]);
		if (xResult != pdPASS) {
			log_err("task creation failed!\n");
			goto err_task;
		}
	}

	/* Print task */
	xResult = xTaskCreate(log_task, "log_task", STACK_SIZE,
				&ctx->rt_ctx, LOWEST_TASK_PRIORITY + 1, &ctx->tc_taskHandles[hnd_idx++]);
	if (xResult != pdPASS) {
		log_err("task creation failed!\n");
		goto err_task;
	}

	ctx->started = true;

	return 0;

err_task:
	/* hnd_idx is being incremented before the task creation */
	for (hnd_idx -= 2; hnd_idx >= 0; hnd_idx--) {
		if (ctx->tc_taskHandles[hnd_idx]) {
			vTaskDelete(ctx->tc_taskHandles[hnd_idx]);
			ctx->tc_taskHandles[hnd_idx] = NULL;
		}
	}

	rt_latency_destroy(&ctx->rt_ctx);

err:
	return -1;
}

void command_handler(void *ctx)
{
	char input;
	uint32_t cmd_id;
	status_t status = (status_t)kStatus_Fail;

	log_raw_info("\r\nTest Case ID (1-6): \r\n");
	log_raw_info("\t1: no extra load\r\n");
	log_raw_info("\t2: extra CPU load\r\n");
	log_raw_info("\t3: extra IRQ load\r\n");
	log_raw_info("\t4: extra CPU load + semaphore load\r\n");
	log_raw_info("\t5: extra CPU load + Linux load (not provided by the test case)\r\n");
	log_raw_info("\t6: extra CPU load + cache flush\r\n");

	do
	{
		log_raw_info("\rPlease input the test case ID (1-6): ");
		status = DbgConsole_TryGetchar(&input);
		if (status != kStatus_Success)
		{
			vTaskDelay(pdMS_TO_TICKS(500));
			continue;
		}
		else
		{
			log_raw_info("%c", input);
		}
	} while ((input != '1') && (input != '2') && (input != '3') &&
		 (input != '4') && (input != '5') && (input != '6'));

	cmd_id = atoi(&input);

	if (cmd_id >= RT_LATENCY_TEST_CASE_MAX) {
		log_err("Wrong test case id!\n");
		return;
	}

	start_test_case(ctx, cmd_id);
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
		if (!ctx->started)
			command_handler(ctx);

		vTaskDelay(pdMS_TO_TICKS(1000));
	} while(1);
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
			&main_ctx, LOWEST_TASK_PRIORITY + 1, NULL);
	assert(xResult == pdPASS);

	/* Start scheduler */
	vTaskStartScheduler();

	for (;;)
		;

	return xResult;
}

