#include "../config.c"
#include "../include/utils/potato_try.h"
#include "../include/judge/result.h"
#include "../include/judge/task.h"
#include "../include/problem/set.h"

#include "../utils/copy_file.c"

#include <stdlib.h>

#define L_LIMIT_FSIZE_MB 1
#define L_LIMIT_NPROC 1

judge_result_t *submission(const judge_task_t *task, int case_count, judge_status_t *err_code)
{
	judge_result_t *ret_err = NULL;
	case_count = -1;

	if (task == NULL) {
		goto err_out;
	}

	char solution_path[MAX_PATH_LENGTH];
	char driver_path[MAX_PATH_LENGTH];
	char sandbox_solution_path[MAX_PATH_LENGTH];
	char sandbox_driver_path[MAX_PATH_LENGTH];
	char problem_config_path[MAX_PATH_LENGTH];

	char buffer[MAX_PATH_LENGTH];

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

	int case_count = -1;
	problem_limit_t l_limit;
	l_limit.file_mb = L_LIMIT_FSIZE_MB;
	l_limit.process = L_LIMIT_NPROC;

	FILE *fp = fopen(problem_config_path, "r");
	if (!fp) {
		*err_code = JUDGE_NO_PROBLEM_CONFIG;
		goto err_out;
	}
	while (fgets(buffer, sizeof(buffer), fp)) {
		sscanf(buffer, "case_count=%d", &case_count);
		sscanf(buffer, "limit_time_s=%d", &(l_limit.time_s));
		sscanf(buffer, "limit_as_mb=%d", &(l_limit.as_mb));
		sscanf(buffer, "limit_stack_mb=%d", &(l_limit.stack_mb));
	}
	fclose(fp);

	for (int id = 0; id < case_count; ++id) {
		// TODO loop to fill problem_set_t and execute it
	}

err_out:
	return ret_err;
}
