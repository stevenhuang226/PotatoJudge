#include "../include/global.h"
#include "../utils/copy_fd.c"
#include "../utils/memfd_create.c"
#include "../include/utils/potato_try.h"
#include "../include/execute/status.h"
#include "../include/execute/resource.h"
#include "../include/problem/set.h"
#include "../include/problem/limit.h"
#include "../include/problem/path.h"

#include <sys/resource.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sched.h>

#include "./isolate.c"

execute_status_t pj_execute_entry(
	sandbox_path_t *sandbox_path,
	problem_set_t *problem_set,
	execute_resource_t *exe_usage)
{
	if (sandbox_path == NULL ||
		problem_set == NULL ||
		exe_usage == NULL) {
		return EXECUTE_UNKNOW;
	}

	char str_shm_fd[32];
	char str_exp_fd[32];
	char str_shm_size[32];
	char str_exp_size[32];

	int input_fd = -1;
	off_t input_size = -1;
	int shm_fd = -1;
	off_t shm_size = problem_set->max_result_size;
	int output_fd = -1;
	off_t output_size = -1;
	int exp_fd = -1;

	execute_status_t ret_err = EXECUTE_UNKNOW;

	TRY_GIVE_ERR(open(problem_set->input_path, O_RDONLY), input_fd, EXECUTE_NO_INPUT);
	TRY_GIVE(lseek(input_fd, 0, SEEK_END), input_size);
	lseek(input_fd, 0, SEEK_SET);

	if (shm_size < input_size) {
		perror("max result size too small");
		goto err_out;
	}

	TRY_GIVE(memfd_create_wrap("pj_shm", shm_size), shm_fd);

	TRY_NOEQU(copy_fd(input_fd, shm_fd), COPY_FD_SUCCESS);

	close(input_fd); input_fd = -1;

	pid_t parent_pid = fork();

	if (parent_pid < 0) {
		perror("fork");
		goto err_out;
	}

	if (parent_pid == 0) {
		execute_status_t ret = pj_execute_isolate(
			sandbox_path,
			problem_set,
			exe_usage,
			shm_fd, shm_size);

		close(shm_fd); shm_fd = -1;
		_exit(ret);
	}

	int parent_status;
	TRY(waitpid(parent_pid, &parent_status, 0));

	if (!WIFEXITED(parent_status)) goto err_out;

	int code = WEXITSTATUS(parent_status);
	if (code != EXECUTE_OK) {
		if (IS_EXECUTE_STATUS(code)) ret_err = code;
		goto err_out;
	}

	TRY_GIVE_ERR(open(problem_set->output_path, O_RDONLY), output_fd, EXECUTE_NO_OUTPUT);
	TRY_GIVE(lseek(output_fd, 0, SEEK_END), output_size);
	lseek(output_fd, 0, SEEK_SET);

	TRY_GIVE(memfd_create_wrap("pj_exp", output_size), exp_fd);
	TRY_NOEQU(copy_fd(output_fd, exp_fd), COPY_FD_SUCCESS);

	close(output_fd); output_fd = -1;

	snprintf(str_shm_fd, sizeof(str_shm_fd), "%d", shm_fd);
	snprintf(str_exp_fd, sizeof(str_exp_fd), "%d", exp_fd);
	snprintf(str_shm_size, sizeof(str_shm_size), "%lld", shm_size);
	snprintf(str_exp_size, sizeof(str_exp_size), "%lld", output_size);

	char *argv[] = {
		problem_set->checker_path,
		str_shm_fd,
		str_exp_fd,
		str_shm_size,
		str_exp_size,
		NULL,
	};

	pid_t checker_pid = fork();

	if (checker_pid < 0) {
		perror("checker fork");
		goto err_out;
	}

	if (checker_pid == 0) {
		execv(problem_set->checker_path, argv);
		_exit(EXECUTE_UNKNOW);
	}

	int checker_status;
	int checker_wait;
	checker_wait = waitpid(checker_pid, &checker_status, 0);

	close(shm_fd); shm_fd = -1;
	close(exp_fd); exp_fd = -1;

	if (checker_wait < 0 || !WIFEXITED(checker_status)) {
		goto err_out;
	}

	int checker_exit = WEXITSTATUS(checker_status);
	if (checker_exit == 0) {
		return EXECUTE_OK;
	} else {
		return EXECUTE_WA;
	}

err_out:
	if (input_fd >= 0) {
		close(input_fd);
		input_fd = -1;
	}
	if (shm_fd >= 0) {
		close(shm_fd);
		shm_fd = -1;
	}
	if (output_fd >= 0) {
		close(output_fd);
		output_fd = -1;
	}
	if (exp_fd >= 0) {
		close(exp_fd);
		exp_fd = -1;
	}

	return ret_err;
}
