#include "../config.c"
#include "../include/judge/compiler.h"
#include "../include/judge/status.h"
#include "../include/judge/task.h"
#include "../utils/copy_file.c"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void *create_shared(ssize_t size)
{
	void *shared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

compile_status_t compile_gcc(const char *sandbox_path)
{
	pid_t child_pid = fork();

	if (child_pid < 0) {
		return COMPILE_BUILD_FAIL;
	}

	if (child_pid == 0) {
		/* child */

		/*
		 * TODO add unshare + chroot
		 */

		if (chdir(sandbox_path) != 0 ) {
			_exit(127);
		}

		struct rlimit cpu_limit;
		cpu_limit.rlim_cur = 2;
		cpu_limit.rlim_max = 2;
		if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
			perror("compile_gcc: setrlimit cpu");
			_exit(127);
		}

		struct rlimit mem_limit;
		mem_limit.rlim_cur = 256 * 1024 * 1024; // 256MB
		mem_limit.rlim_max = 256 * 1024 * 1024;
		if (setrlimit(RLIMIT_AS, &mem_limit) != 0) {
			perror("compile_gcc: setrlimit memory");
			_exit(127);
		}

		execlp("gcc",
				"gcc",
				"solution.c",
				"driver.c",
				"-std=c99",
				"-o", "a.out",
				NULL);

		perror("compil_gcc: child error");
		_exit(127);
	}

	int status;
	if (waitpid(child_pid, &status, 0) < 0) {
		return COMPILE_BUILD_FAIL;
	}

	if (WIFEXITED(status)) {
		int exit_code = WEXITSTATUS(status);

		if (exit_code == 0) {
			return COMPILE_SUCCESS;
		} else if (exit_code == 127) {
			return COMPILE_BUILD_FAIL;
		} else {
			return COMPILE_FAIL;
		}
	}

	if (WIFSIGNALED(status)) {
		return COMPILE_BUILD_FAIL;
	}

	return COMPILE_BUILD_FAIL;
}

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
			g_judge_config.sandbox_path);
	snprintf(sandbox_driver_path, sizeof(sandbox_driver_path),
			"%s/driver.c",
			g_judge_config.sandbox_path);

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
}

/* sandbox{compile driver + user_code} => create shared => load input.bin->shared => sandbox{run judge, driver give input and deal return} => checker check shared => return result */
judge_status_t judge_submission(judge_task_t)
{
}
