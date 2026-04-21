#include "../config.c"
#include "../include/sandbox_path.h"
#include "../include/judge/result.h"
#include "../include/judge/task.h"
#include "../include/utils/potato_try.h"

#include "./init.c"
#include "./submission.c"

int main()
{
	int ret_err = -1;
	TRY(initialization());


	judge_status_t err_code = JUDGE_UNKNOW_ERROR;
	int case_count = 0;

	judge_task_t task;
	task.compiler_type = COMPILER_GCC;
	task.submission_id = 0;
	task.problem_id = 0;

	judge_result_t *result = submission(&task, &case_count, &err_code);
	if (result == NULL) {
		printf("FAIL\n");
		switch (err_code) {
		case JUDGE_COMPILE_ERROR:
			printf("compile error\n");
			goto err_out;
		case JUDGE_UNKNOW_ERROR:
			printf("unknow error\n");
			goto err_out;
		case JUDGE_NO_SOLUTION:
			printf("no solution\n");
			goto err_out;
		case JUDGE_NO_DRIVER:
			printf("no driver\n");
			goto err_out;
		case JUDGE_NO_PROBLEM_CONFIG:
			printf("no problem config\n");
			goto err_out;
		default:
			printf("other\n");
			goto err_out;
		}
		goto err_out;
	}

	for (int i = 0; i < case_count; ++i) {
		judge_result_t *res = &result[i];
		printf("judge id: %d\n", res->id);
		switch(res->status) {
			case JUDGE_ACCEPT:
				printf("judge accept\n");
				break;
			case JUDGE_TIME_LIMIT:
				printf("time limit\n");
				break;
			case JUDGE_MEMORY_LIMIT:
				printf("memory limit\n");
				break;
			case JUDGE_SIGSEGV:
				printf("sigsegv\n");
				break;
			case JUDGE_SIGSYS:
				printf("sigsys\n");
				break;
			case JUDGE_WRONG_ANSWER:
				printf("wrong answer\n");
				break;
			case JUDGE_UNKNOW_ERROR:
				printf("unknow error\n");
				break;
			default:
				printf("other\n");
		}

		if (res->status == JUDGE_ACCEPT) {
			printf("judge accept\n");
		} else {
		}
		printf("cpu: %d, mem: %d\n", res->usage.time_us, res->usage.mem_kb);
	}

	free(result);

	return 0;

err_out:
	return ret_err;
}
