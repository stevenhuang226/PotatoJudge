#include "../include/global.h"
#include "../include/utils/potato_try.h"
#include "../include/sandbox_path.h"
#include "../include/problem/set.h"
#include "../include/execute/status.h"
#include "../include/execute/resource.h"

#include "./judge.c"

#include <sys/resource.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>

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
		perror("judge fork");
		goto err_out;
	}

	if (judge_pid == 0) {
		execute_status_t ret = execute_judge(
			sandbox_path,
			&(problem_set->limit),
			shm_fd, shm_size);
		close(shm_fd); shm_fd = -1;
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
