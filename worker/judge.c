#include "../config.c"
#include "../include/config.h"
#include "../include/utils/potato_try.h"
#include "../include/judge/task.h"
#include "../include/judge/result.h"

#include"./submission.c"

#include <stdio.h>

#define SUB_CONFIG_COMPILER_TYPE "compiler_type=%d"
int8_t frk_judge(uint32_t submission_id, uint32_t problem_id)
{
	int8_t ret_err = -1;

	char buffer[4096];
	char str_sub_config_path[MAX_PATH_LENGTH];
	snprintf(str_sub_config_path, sizeof(str_sub_config_path),
		"%s/%u/detail.conf",
		g_judge_config.base_submission, submission_id);


	judge_task_t task;
	task.submission_id = submission_id;
	task.problem_id = problem_id;

	FILE *fp;
	TRY_GIVE(fopen(str_sub_config_path, "r"), fp);

	while (fgets(buffer, sizeof(buffer), fp)) {
		sscanf(buffer, SUB_CONFIG_COMPILER_TYPE, &task.compiler_type);
	}
	fclose(fp);

	TRY(IS_COMPILER_TYPE(task.compiler_type));

	int case_count = -1;
	judge_status_t err_code;
	judge_result_t *result = submission(&task, &case_count, &err_code);

	/*
	 * write response out
	 */
err_out:
	return ret_err;
}
