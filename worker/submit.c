#include "../config.c"
#include "../include/utils/potato_try.h"
#include "../include/judge/result.h"
#include "../include/judge/task.h"
#include "../include/problem/set.h"

#include "../utils/copy_file.c"

#include "../compile/entry.c"
#include "../execute/entry.c"

#include <stdlib.h>

/*
 * the return array judge_result_t* size of *ret_case_count.
 * *err_code: when return array == NULL. Check this value. If return isn't NULL, this value should write as judge_accept
 */

#define L_LIMIT_FSIZE_MB 1 // limit FSIZE
#define L_LIMIT_NPROC 1 // limit NPROC
#define PROBLEM_CONFIG_CASE_COUNT "case_count=%d"		// case count // these should match problem/X/config.conf
#define PROBLEM_CONFIG_CPU "limit_time_s=%d"			// cpu time limit
#define PROBLEM_CONFIG_AS "limit_as_mb=%d"			// memory limit
#define PROBLEM_CONFIG_STACK "limit_stack_mb=%d"		// stack limit
#define PROBLEM_CONFIG_MAX_RES "expect_max_result_mb=%lld"	// maximum result expect size (using for set shared memory)

judge_result_t *pj_submit(const judge_task_t *task, int *ret_case_count, judge_status_t *err_code)
{
	judge_result_t *ret_err = NULL;
	int case_count = -1;

	if (task == NULL || ret_case_count == NULL || err_code == NULL) {
		goto err_out;
	}

	char solution_path[MAX_PATH_LENGTH];
	char driver_path[MAX_PATH_LENGTH];
	char sandbox_solution_path[MAX_PATH_LENGTH];
	char sandbox_driver_path[MAX_PATH_LENGTH];
	char problem_config_path[MAX_PATH_LENGTH];

	char buffer[4096];

	snprintf(solution_path, sizeof(solution_path),
		"%s/%u/solution.c",
		g_judge_config.base_submission, task->submission_id);
	snprintf(driver_path, sizeof(driver_path),
		"%s/%u/driver.c",
		g_judge_config.base_problem, task->problem_id);
	snprintf(sandbox_solution_path, sizeof(sandbox_solution_path),
		"%s/solution.c",
		g_judge_config.sandbox_path.base);
	snprintf(sandbox_driver_path, sizeof(sandbox_driver_path),
		"%s/driver.c",
		g_judge_config.sandbox_path.base);
	snprintf(problem_config_path, sizeof(problem_config_path),
		"%s/%u/config.conf",
		g_judge_config.base_problem, task->problem_id);

	copy_file_t cp_sln = copy_file(solution_path, sandbox_solution_path);
	if (cp_sln == COPY_FILE_NOT_FOUND) {
		*err_code = JUDGE_NO_SOLUTION;
		goto err_out;
	}
	if (cp_sln == COPY_FILE_FAILED) {
		*err_code = JUDGE_UNKNOW_ERROR;
		goto err_out;
	}

	copy_file_t cp_drv = copy_file(driver_path, sandbox_driver_path);
	if (cp_drv == COPY_FILE_NOT_FOUND) {
		*err_code = JUDGE_NO_DRIVER;
		goto err_out;
	}
	if (cp_drv == COPY_FILE_FAILED) {
		*err_code = JUDGE_UNKNOW_ERROR;
		goto err_out;
	}

	compile_status_t compile_stat
		= pj_compile_entry(&g_judge_config.sandbox_path, task->compiler_type);
	switch (compile_stat) {
	case COMPILE_FAIL:
		*err_code = JUDGE_COMPILE_ERROR;
		goto err_out;
	case COMPILE_NO_SOLUTION:
		*err_code = JUDGE_NO_SOLUTION;
		goto err_out;
	case COMPILE_NO_DRIVER:
		*err_code = JUDGE_NO_DRIVER;
		goto err_out;
	case COMPILE_UNKNOW:
		*err_code = JUDGE_UNKNOW_ERROR;
		goto err_out;
	}

	problem_limit_t l_limit;
	l_limit.file_mb = L_LIMIT_FSIZE_MB;
	l_limit.process = L_LIMIT_NPROC;
	off_t max_result_size = -1;

	FILE *fp = fopen(problem_config_path, "r");
	if (!fp) {
		*err_code = JUDGE_NO_PROBLEM_CONFIG;
		goto err_out;
	}
	while (fgets(buffer, sizeof(buffer), fp)) {
		sscanf(buffer, PROBLEM_CONFIG_CASE_COUNT, &case_count);
		sscanf(buffer, PROBLEM_CONFIG_CPU, &(l_limit.time_s));
		sscanf(buffer, PROBLEM_CONFIG_AS, &(l_limit.as_mb));
		sscanf(buffer, PROBLEM_CONFIG_STACK, &(l_limit.stack_mb));
		sscanf(buffer, PROBLEM_CONFIG_MAX_RES, &max_result_size);
	}
	fclose(fp);

	judge_result_t *result = malloc(case_count * sizeof(judge_result_t));
	if (result == NULL) {
		*err_code = JUDGE_UNKNOW_ERROR;
		goto err_out;
	}

	for (int id = 0; id < case_count; ++id) {
		problem_set_t problem_set;

		problem_set.limit.time_s = l_limit.time_s;
		problem_set.limit.as_mb = l_limit.as_mb;
		problem_set.limit.stack_mb = l_limit.stack_mb;
		problem_set.limit.file_mb = l_limit.file_mb;
		problem_set.limit.process = l_limit.process;

		problem_set.max_result_size = max_result_size;

		snprintf(problem_set.input_path, sizeof(problem_set.input_path),
			"%s/%u/input%d.bin",
			g_judge_config.base_problem, task->problem_id, id);
		snprintf(problem_set.output_path, sizeof(problem_set.output_path),
			"%s/%u/output%d.bin",
			g_judge_config.base_problem, task->problem_id, id);
		snprintf(problem_set.checker_path, sizeof(problem_set.checker_path),
			"%s/%u/checker.out",
			g_judge_config.base_problem, task->problem_id);

		execute_resource_t usage;
		execute_status_t ret = pj_execute_entry(&g_judge_config.sandbox_path, &problem_set, &usage);

		judge_status_t js;

		switch (ret) {
		case EXECUTE_OK:
			js = JUDGE_ACCEPT;
			break;
		case EXECUTE_WA:
			js = JUDGE_WRONG_ANSWER;
			break;
		case EXECUTE_SIGSEGV:
			js = JUDGE_SIGSEGV;
			break;
		case EXECUTE_SIGSYS:
			js = JUDGE_SIGSYS;
			break;
		case EXECUTE_TLE:
			js = JUDGE_TIME_LIMIT;
			break;
		case EXECUTE_MLE:
			js = JUDGE_MEMORY_LIMIT;
			break;
		case EXECUTE_UNKNOW:
		default:
			js = JUDGE_UNKNOW_ERROR;
			break;
		}

		result[id].id = id;
		result[id].status = js;
		result[id].usage.time_us = usage.time_us;
		result[id].usage.mem_kb = usage.mem_kb;
	}

	*err_code = JUDGE_ACCEPT;
	*ret_case_count = case_count;
	return result;

err_out:
	return ret_err;
}
