#include "../config.c"
#include "../include/judge/compiler.h"
#include "../include/judge/status.h"
#include "../include/judge/task.h"
#include "../include/judge/sandbox_path.h"
#include "../utils/copy_file.c"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "../compile/compile.c";

compile_status_t judge_prepare(const judge_task_t *task)
{
	if (task == NULL) {
		return COMPILE_BUILD_FAIL;
	}

	char solution_path[MAX_PATH_LENGTH];
	char driver_path[MAX_PATH_LENGTH];

	char sandbox_solution_path[MAX_PATH_LENGTH];
	char sandbox_driver_path[MAX_PATH_LENGTH];

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

	copy_file_t cp_solution = copy_file(solution_path, sandbox_solution_path);
	if (cp_solution == COPY_FILE_NOT_FOUND) {
		return COMPILE_SOLUTION_NOT_FOUND;
	}
	if (cp_solution == COPY_FILE_FAILED) {
		return COMPILE_BUILD_FAIL;
	}
	copy_file_t cp_driver = copy_file(driver_path, sandbox_driver_path);
	if (cp_driver == COPY_FILE_NOT_FOUND) {
		return COMPILE_DRIVER_NOT_FOUND;
	}
	if (cp_driver == COPY_FILE_FAILED) {
		return COMPILE_BUILD_FAIL;
	}

	return COMPILE_SUCCESS;
}

/* sandbox{compile driver + user_code} => create shared => load input.bin->shared => \
 * sandbox{run judge, driver give input and deal return} => checker check shared => return result */
judge_status_t judge_submission(judge_task_t *task)
{
	compile_status_t prepare = judge_prepare(task);
	if (prepare != COMPILE_SUCCESS) {
		return JUDGE_UNKNOW_ERROR;
	}

	compile_status_t compile_result;
	if (task->compiler_type == COMPILER_GCC) {
		compile_result = compile_gcc(&g_judge_config.sandbox_path);
	}

	if (compile_result == COMPILE_FAIL) {
		return JUDGE_COMPILE_ERROR;
	}
	if (compile_result != COMPILE_SUCCESS) {
		return JUDGE_UNKNOW_ERROR;
	}

	/* tmp */
	return JUDGE_ACCEPT;
	/* end tmp */
}


/* tmp test */

#include "./worker.c"

int main()
{
	if (initialization() < 0) {
		printf("FAIL\n");
		return 0;
	}

	judge_task_t task;
	task.submission_id = 0;
	task.problem_id = 0;
	task.compiler_type = COMPILER_GCC;

	judge_status_t judge_result = judge_submission(&task);


	if (judge_result == JUDGE_ACCEPT) {
		printf("COMPILE SUCCES\n");
	} else {
		printf("FAIL\n");
	}

	return 0;
}

/* end tmp */
