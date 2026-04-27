#include "./include/global.h"
#include "./include/config.h"

#define CONF_PATH "/tmp/pj/pj_config.conf"		// path to config file

/* problem */
#define INPUT_PREFIX "input"
#define INPUT_SUFFIX ".bin"
#define OUTPUT_PREFIX "output"
#define OUTPUT_SUFFIX ".bin"

#define SOLUTION_NAME "solution"
#define DRIVER_NAME "driver"
#define PROBLEM_CONFIG_NAME "config.conf"
#define CHECKER_NAME "checker.out"

#define PROBLEM_CASE_COUNT "case_count"			// case count
#define PROBLEM_CPU_SECOND "limit_time_s"		// cpu time limit (second)
#define PROBLEM_MEM_MB "limit_as_mb"			// memory limit (mb)
#define PROBLEM_STACK_MB "limit_stack_mb"		// stack limit (mb)
#define PROBLEM_SHM_MB "expect_max_result_mb"		// SHM alloc size (mb)
		/* PROBLEM_SHM_MB == MAX(expect max result, input) size */
#define PROBLEM_DEF_FSIZE 1
#define PROBLEM_DEF_NPROC 1

/* compile */
#define COMPILED_OUT_NAME "a.out"

/* submission */
#define SUB_DETAIL_NAME "detail.conf"			// provie compiler type
#define SUB_COMPILER_TYPE "compiler_type"
/*
 * [1] gcc
 * [2] g++
 */

/* result */
#define RES_WRK_NAME "judge.json"
#define RES_DONE_NAME "done.json"

/* listener */
#define SOCKET_PATH "/tmp/pj.sock"				// the socket name. Will used to listen submit call

judge_config_t g_judge_config;
