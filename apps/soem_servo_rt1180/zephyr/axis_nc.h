/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AXIS__NC____H__
#define __AXIS__NC____H__
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/* TP Arrays Parser Error code definitions */
#define AXIS_PARSER_SUCCESS               0
#define AXIS_PARSER_ERR_CYCLIC_ERROR      1
#define AXIS_PARSER_ERR_SCALE_ERROR       2
#define AXIS_PARSER_ERR_BIAS_ERROR        3
#define AXIS_PARSER_ERR_ACCEL_ERROR       4
#define AXIS_PARSER_ERR_DECEL_ERROR       5
#define AXIS_PARSER_ERR_MAX_SPEED_ERROR   6
#define AXIS_PARSER_ERR_PARSE_ERROR       7
#define AXIS_PARSER_ERR_NO_TRAJECTORY     8
#define AXIS_PARSER_ERR_TOO_MANY_POINTS   9
#define AXIS_PARSER_ERR_MODE_ERROR        10

#define TRAJECTORY_PLANNER_MAX  18
#define SOEM_NSEC_PER_SEC (1000000000)
#define timespec_add(a, b, result)                          \
    do {                                                      \
        (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;           \
        (result)->tv_nsec = (a)->tv_nsec + (b)->tv_nsec;        \
        if ((result)->tv_nsec >= SOEM_NSEC_PER_SEC)                  \
        {                                                       \
            ++(result)->tv_sec;                             \
            (result)->tv_nsec -= SOEM_NSEC_PER_SEC;              \
        }                                                       \
    } while (0)

struct trajectory_planner_t
{
	uint32_t points_num;
	uint32_t current_point;
	double origin_pos;
	double target_pos;
	double uniform_speed;
	double accel_pos;
	double decel_pos;
	uint32_t point_accel_end;
	uint32_t point_decel_start;
	int32_t direction;
};

typedef enum {
	csp_status_stop = 0,
	csp_status_ready = 1,
	csp_status_running = 2,
	csp_status_pre_stop = 3,
} csp_status_t;

struct axis_csp_status_t {
	int32_t bias;
	uint32_t tp_num;
	uint32_t is_cyclic;
	uint32_t current_tp;
	double acceleration;
	double deceleration;
	double max_speed;
	csp_status_t csp_status;
	struct trajectory_planner_t tp[TRAJECTORY_PLANNER_MAX];
};

struct axis_t {
	struct servo_t *servo;
	int axis_offset;
	uint8_t *output_offset;
	uint8_t *input_offset;
	char *tp;
	uint8_t mode;
	int32_t current_position;
	int32_t current_velocity;
	uint32_t scale;
	struct axis_csp_status_t axis_status;
};

int32_t axis_nc_get_next_pos(struct axis_t *axis);
int axis_nc_slave_check(struct axis_t *axis, int axis_num);
int axis_nc_synced_check(struct axis_t *axis, int axis_num);
int axis_nc_init(struct axis_t *axis, char *tp, uint32_t period_ns);
int axis_nc_start(struct axis_t *axis);
int axis_nc_stop(struct axis_t *axis);

#endif
