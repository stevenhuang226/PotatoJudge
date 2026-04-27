#include "../config.c"
#include "../include/config.h"
#include "../include/utils/potato_try.h"
#include "../include/judge/task.h"
#include "../include/judge/result.h"

#include "./submit.c"
#include "./write_result.c"

#include <stdio.h>

int8_t pj_judge_entry(uint32_t submission_id, uint32_t problem_id)
{
	int8_t ret_err = -1;

	char buffer[4096];
	char str_sub_config_path[MAX_PATH_LENGTH];
	char str_output_dir[MAX_PATH_LENGTH];
	snprintf(str_sub_config_path, sizeof(str_sub_config_path),
		"%s/%u/" SUB_DETAIL_NAME,
		g_judge_config.base_submission, submission_id);
	snprintf(str_output_dir, sizeof(str_output_dir),
		"%s/%u",
		g_judge_config.base_submission, submission_id);

	judge_task_t task;
	task.submission_id = submission_id;
	task.problem_id = problem_id;
	task.compiler_type = COMPILER_NULL;

	FILE *fp = fopen(str_sub_config_path, "r");
	if (! fp) goto err_out;
	while (fgets(buffer, sizeof(buffer), fp)) {
		sscanf(buffer, SUB_COMPILER_TYPE "=%d", &task.compiler_type);
	}
	fclose(fp);

	TRY(IS_COMPILER_TYPE(task.compiler_type));

	int case_count = -1;
	judge_status_t err_code;
	judge_result_t *result = pj_submit(&task, &case_count, &err_code);

	if (result == NULL && ! IS_JUDGE_STATUS(err_code)) {
		goto err_out;
	}

	if (result == NULL && IS_JUDGE_STATUS(err_code)) {
		result = malloc(sizeof(judge_result_t));
		result->id = -1;
		result->status = err_code;
		result->usage.time_us = 0;
		result->usage.mem_kb = 0;
		case_count = 1;
	}

	TRY(pj_write_result(result, case_count, str_output_dir));
	free(result);

	return 0;
err_out:
	return ret_err;
}
