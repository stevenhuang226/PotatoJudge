#include "../config.c"
#include "../utils/copy_fd.c"
#include "../utils/memfd_create.c"
#include "./set_limit.c"

#include "../include/utils/potato_try.h"
#include "../include/judge/execute.h"
#include "../include/problem/problem_set.h"

#include <sys/resource.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>

#define COMPILED_USER_CODE_NAME "./a.out"
execute_status_t execute_judge(
	sandbox_path_t *sandbox_path,
	problem_limit_t *problem_limit,
	int shm_fd, off_t shm_size)
{
	if (sandbox_path == NULL ||
		problem_limit == NULL ||
		shm_fd < 0 || shm_size < 0) {
		goto err_out;
	}

	execute_status_t ret_err = EXECUTE_UNKNOW;
	char str_shm_fd[32];
	char str_shm_size[32];
	snprintf(str_shm_fd, sizeof(str_shm_fd), "%d", shm_fd);
	snprintf(str_shm_size, sizeof(str_shm_size), "%lld", shm_size);

	TRY(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL));
	TRY(chdir(sandbox_path->base) < 0);
	TRY(chdir("."));
	TRY(chdir("/"));

	TRY(set_limit(problem_limit));
	TRY(set_seccomp());

	char *argv[] = {
		COMPILED_USER_CODE_NAME,
		str_shm_fd,
		str_shm_size,
		NULL,
	};

	execv(COMPILED_USER_CODE_NAME, argv);

	_exit(EXECUTE_UNKNOW);
err_out:
	if (shm_fd >= 0) close(shm_fd); shm_fd = -1;
	_exit(ret_err);
}

execute_status_t execute_isolate(
	sandbox_path_t *sandbox_path,
	problem_set_t *problem_set,
	execute_resource_t *exe_usage,
	int shm_fd, off_t shm_size)
{
	if (sandbox_path == NULL ||
		problem_set == NULL ||
		exe_usage == NULL ||
		shm_fd < 0 || shm_size < 0) {
		goto err_out;
	}

	char str_setgroups[] = "deny\n";
	char str_uidmap[32];
	char str_gidmap[32];
	int fd;

	snprintf(str_uidmap, sizeof(str_uidmap), "0 %u 1\n", getuid());
	snprintf(str_gidmap, sizeof(str_gidmap), "0 %u 1\n", getgid());

	TRY(unshare(CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID |
		CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNET));

	TRY_GIVE(open("/proc/self/setgroups", O_WRONLY), fd);
	TRY(write(fd, str_setgroups, strlen(str_setgroups)));
	close(fd);

	TRY_GIVE(open("/proc/self/uid_map", O_WRONLY), fd);
	TRY(write(fd, str_uidmap, strlen(str_uidmap)));
	close(fd);

	TRY_GIVE(open("/proc/self/gid_map", O_WRONLY), fd);
	TRY(write(fd, str_gidmap, strlen(str_gidmap)));
	close(fd);

	TRY(setreuid(0, 0));
	TRY(setregid(0, 0));

	pid_t judge_pid = fork();

	if (judge_pid < 0) {
		perror("fork");
		goto err_out;
	}

	if (judge_pid == 0) {
		execute_status_t ret = execute_judge(
			sandbox_path,
			&(problem_set->limit),
			shm_fd, shm_size);

		close(shm_fd);
		_exit(ret);
	}

	int status;
	struct rusage usage;

	TRY(wait4(judge_pid, &status, 0, &usage));

	uint64_t time_us = usage.ru_utime.tv_sec * 1000000ULL + usage.ru_utime.tv_usec +
		usage.ru_stime.tv_sec * 1000000ULL + usage.ru_stime.tv_usec;
	uint64_t mem_kb = usage.ru_maxrss;

	exe_usage->time_us = time_us; exe_usage->mem_kb = mem_kb;

	if (WIFEXITED(status)) return WEXITSTATUS(status);

	if (WIFSIGNALED(status)) {
		int sig = WTERMSIG(status);

		if (sig == SIGXCPU) {
			return EXECUTE_TLE;
		} if (sig == SIGSYS) {
			return EXECUTE_SIGSYS;
		} if (sig == SIGSEGV) {
			if (mem_kb * 1000ULL > problem_set->limit.as_mb) {
				return EXECUTE_MLE;
			} else {
				return EXECUTE_SIGSEGV;
			}
		} if (sig == SIGKILL) {
			return EXECUTE_SIGKILL;
		}

		return EXECUTE_UNKNOW;
	}

	return EXECUTE_UNKNOW;
err_out:
	if (shm_fd >= 0) close(shm_fd); shm_fd = -1;
	return EXECUTE_UNKNOW;
}

execute_status_t execute(
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
	int shm_fd = -1;
	off_t shm_size = problem_set->max_result_size;
	int output_fd = -1;
	off_t output_size = -1;
	int exp_fd = -1;

	execute_status_t ret_err = EXECUTE_UNKNOW;

	TRY_GIVE_ERR(open(problem_set->input_path, O_RDONLY), input_fd, EXECUTE_NO_INPUT);

	TRY_GIVE(memfd_create_wrap("pj_shm", problem_set->max_result_size), shm_fd);
	TRY_NOEQU(copy_fd(input_fd, shm_fd), COPY_FD_SUCCESS);

	close(input_fd); input_fd = -1;

	pid_t parent_pid = fork();

	if (parent_pid < 0) {
		perror("fork");
		goto err_out;
	}

	if (parent_pid == 0) {
		execute_status_t ret = execute_isolate(
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
