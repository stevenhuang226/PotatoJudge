#include "../config.c"
#include "../include/judge/compiler.h"
#include "../include/judge/status.h"
#include "../include/judge/task.h"
#include <stdlib.h>
#include <sys/mman.h>

void *create_shared(ssize_t size)
{
	void *shared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

compile_status_t judge_compile(const judge_task_t *task)
{
	char solution_path[MAX_PATH_LENGTH];
	char driver_path[MAX_PATH_LENGTH];

	snprintf(solution_path, sizeof(solution_path),
			"%s/%d/solution.c",
			g_judge_config.base_submission, task->submission_id);
	snprintf(driver_path, sizeof(driver_path),
			"%s/%s/driver.c",
			g_judge_config.base_problem, task->problem_id);

	FILE *solution_fp = fopen(solution_path, "r");
	if (! solution_fp) {
		return COMPILE_SOLUTION_NOT_FOUND;
	}

	FILE *driver_fp = fopen(driver_path, "r");
	if (! driver_fp) {
		return COMPILE_DRIVER_NOT_FOUND;
	}
}

/* sandbox{compile driver + user_code} => create shared => load input.bin->shared => sandbox{run judge, driver give input and deal return} => checker check shared => return result */
judge_status_t judge_submission(judge_task_t)
{
}
