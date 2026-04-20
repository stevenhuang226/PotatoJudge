#include "./include/global.h"
#include "./include/config.h"

#define CONF_PATH "/tmp/pj/pj_config.conf"

#define SUB_DETAIL_NAME "detail.conf"
#define SUB_CONFIG_COMPILER_TYPE "compiler_type=%d"

#define L_LIMIT_FSIZE_MB 1 // limit FSIZE
#define L_LIMIT_NPROC 1 // limit NPROC
#define PROBLEM_CONFIG_CASE_COUNT "case_count=%d"		// case count // these should match problem/X/config.conf
#define PROBLEM_CONFIG_CPU "limit_time_s=%d"			// cpu time limit
#define PROBLEM_CONFIG_AS "limit_as_mb=%d"			// memory limit
#define PROBLEM_CONFIG_STACK "limit_stack_mb=%d"		// stack limit
#define PROBLEM_CONFIG_MAX_RES "expect_max_result_mb=%lld"	// maximum result expect size (using for set shared memory)

judge_config_t g_judge_config;
