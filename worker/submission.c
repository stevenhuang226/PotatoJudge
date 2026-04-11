#include "../config.c"
#include "../include/utils/potato_try.h"
#include "../include/judge/status.h"
#include "../include/judge/task.h"

judge_status_t *submission(const judge_task_t *task, int case_count)
{
	judge_status_t ret_err = NULL;
	case_count = -1;

	if (task == NULL) {
		goto err_out;
	}

err_out:
	return ret_err;
}
