#include "../config.c"
#include "../include/judge/compiler.h"
#include "../include/judge/status.h"
#include "../include/judge/task.h"
#include "../utils/copy_file.c"

#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>


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

		if (unshare(CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWPID) < 0) {
			perror("unshare fail");
			exit(127);
		}

		pid_t inner_pid = fork();
		
		if (inner_pid < 0) {
			perror("fork 2");
			exit(127);
		}

		if (inner_pid > 0) {
			int inner_status;
			waitpid(inner_pid, &inner_status, 0);
			exit(inner_status);
		}

		char sandbox_path_usr[MAX_PATH_LENGTH];
		char sandbox_path_lib[MAX_PATH_LENGTH];
		char sandbox_path_lib64[MAX_PATH_LENGTH];
		char sandbox_path_tmp[MAX_PATH_LENGTH];
		char sandbox_path_dev_nll[MAX_PATH_LENGTH];

		snprintf(sandbox_path_usr, sizeof(sandbox_path_usr),
				"%s/usr",
				sandbox_path);
		snprintf(sandbox_path_lib, sizeof(sandbox_path_lib),
				"%s/lib",
				sandbox_path);
		snprintf(sandbox_path_lib64, sizeof(sandbox_path_lib64),
				"%s/lib64",
				sandbox_path);
		snprintf(sandbox_path_tmp, sizeof(sandbox_path_usr),
				"%s/tmp",
				sandbox_path);
		snprintf(sandbox_path_dev_nll, sizeof(sandbox_path_dev_nll),
				"%s/dev/null",
				sandbox_path);

		if(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) < 0) {
			perror("mount private");
			exit(127);
		}

		if (mount("/usr", sandbox_path_usr, NULL, MS_REC | MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox_path_usr, NULL, MS_REC | MS_REMOUNT | MS_RDONLY | MS_NODEV, NULL) < 0) {
			perror("mount usr");
			exit(127);
		}

		if (mount("/lib", sandbox_path_lib, NULL, MS_REC | MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox_path_lib, NULL, MS_REC | MS_REMOUNT | MS_RDONLY | MS_NODEV, NULL) < 0) {
			perror("mount lib");
			exit(127);
		}

		if (mount("/lib64", sandbox_path_lib64, NULL, MS_REC | MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox_path_lib64, NULL, MS_REC | MS_REMOUNT | MS_RDONLY | MS_NODEV, NULL) < 0) {
			perror("mount lib64");
			exit(127);
		}

		if (mount("tmpfs", sandbox_path_tmp, "tmpfs", 0, NULL) < 0) {
			perror("mount tmpfs");
			exit(127);
		}

		if (mount("/dev/null", sandbox_path_dev_nll, NULL, MS_BIND, NULL) < 0) {
			perror("mount dev null");
			exit(127);
		}

		if (chroot(sandbox_path) < 0) {
			perror("chroot");
			exit(127);
		}

		chdir("/");

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
				"/usr/lib/gcc",
				"solution.c",
				"driver.c",
				"-std=c99",
				"-o", "a.out",
				NULL);

		perror("run gcc error");
		exit(127);
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

	return COMPILE_SUCCESS;
}

/* sandbox{compile driver + user_code} => create shared => load input.bin->shared => sandbox{run judge, driver give input and deal return} => checker check shared => return result */
judge_status_t judge_submission(judge_task_t *task)
{
	compile_status_t prepare = judge_prepare(task);
	/* tmp */
	if (prepare == COMPILE_SOLUTION_NOT_FOUND) {
		perror("solution not found");
	}
	if (prepare == COMPILE_DRIVER_NOT_FOUND) {
		perror("driver not found");
	}
	/* end tmp */
	if (prepare != COMPILE_SUCCESS) {
		return JUDGE_UNKNOW_ERROR;
	}

	compile_status_t compile_result;
	if (task->compiler_type == COMPILER_GCC) {
		compile_result = compile_gcc(g_judge_config.sandbox_path);
	}

	if (compile_result == COMPILE_FAIL) {
		return JUDGE_COMPILE_ERROR;
	}
	if (compile_result != COMPILE_SUCCESS) {
		return JUDGE_UNKNOW_ERROR;
	}

	/* tmp */
	return JUDGE_ACCEPT;
}


/* tmp test */

#include "./worker.c"

int main()
{
	initialization();

	judge_task_t task;
	task.submission_id = 0;
	task.problem_id = 0;
	task.compiler_type = COMPILER_GCC;

	judge_status_t judge_result = judge_submission(&task);
	if (judge_result == JUDGE_ACCEPT) {
		printf("TEST SUCCES\n");
	} else {
		printf("FAIL\n");
	}

	return 0;
}
