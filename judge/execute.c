#include "../config.c"
#include "../include/judge/execute.h"
#include "../include/judge/sandbox_path.h"
#include "../include/judge/job.h"
#include "../include/problem/problem_set.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

execute_status_t execute(sandbox_path_t *sandbox, problem_set_t *problem_set)
{
	int job_count;

	job_queue_t job_queue;
	job_queue.count = problem_set->job_count;
	job_queue.jobs;

	for (int i = 0; i < job_queue.count; ++i) {
		char path[MAX_PATH_LENGTH];
		snprintf(path, sizeof(path),
			"%s/input%d.bin",
			problem_set->base_path, i);

		int fd = open(path, O_RDONLY);
		if (fd < 0) {
			perror("execute open input file");
			for (int i2 = 0; i2 < i; ++i2) {
				free(job_queue.jobs[i]);
			}
			return EXECUTE_UNKNOW;
		}

		off_t file_size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		(job_queue.jobs)[i] = mmap(NULL, file_size,
			PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
			-1, 0);

		if (job_queue.jobs[i] == NULL) {
			close(fd);
			perror("execute mmap");
			for (int i2 = 0; i2 < i; ++i2) {
				free(job_queue.jobs[i]);
			}
			return EXECUTE_UNKNOW;
		}

		int read_size = read(fd, job_queue.jobs[i], file_size);
		close(fd);

		if (read_size != file_size) {
			return (EXECUTE_UNKNOW);
		}
	}

	int supervisor_pid = fork();

	if (supervisor_pid == 0) {
		uint32_t host_uid = getuid();
		uint32_t host_gid = getgid();

		if (unshare(CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID |
			CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNET) < 0) {
			perror("execute unshare");
			_exit(127);
		}

		int sandbox_pid = fork();

		if (sandbox_pid != 0) {
			int status;
			if (waitpid(sandbox_pid, &status, 0) < 0) _exit(127);
			if (! WIFEXITED(status)) _exit(127);
			_exit(WEXITSTATUS(status));
		}

		char uid_map[64];
		char gid_map[64];
		char setgroups[] = "deny\n";
		snprintf(uid_map, sizeof(uid_map), "0 %u 1", host_uid);
		snprintf(gid_map, sizeof(gid_map), "0 %u 1", host_gid);

		int fd;
		fd = open("/proc/self/setgroups", O_WRONLY);
		if (fd < 0 || write(fd, setgroups, strlen(setgroups)) < 0) {
			perror("open/set setgroups");
			_exit(127);
		}
		close(fd);

		fd = open("/proc/self/uid_map", O_WRONLY);
		if (fd < 0 || write(fd, uid_map, strlen(uid_map)) < 0) {
			perror("open/set uid_map");
			_exit(127);
		}
		close(fd);

		fd = open("/proc/self/gid_map", O_WRONLY);
		if (fd < 0 || write(fd, gid_map, strlen(gid_map)) < 0) {
			perror("open/set gid_map");
			_exit(127);
		}
		close(fd);

		if (setreuid(0, 0) < 0 || setregid(0, 0) < 0) {
			perror("setre uid/gid");
			_exit(127);
		}

		struct rlimit cpu_limit = {
			problem_set->time_limit,
			problem_set->time_limit,
		};

		struct rlimit mem_limit = {
			problem_set->memory_limit * 1024 * 1024, // MB
			problem_set->memory_limit * 1024 * 1024,
		};

		if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0 ||
			setrlimit(RLIMIT_AS, &mem_limit) != 0) {
			perror("set limit");
			_exit(127);
		}
	}

	for (int i = 0; i < job_queue.count; ++i) {
		free(job_queue.jobs[i]);
	}
}
/*
execute_status_t execute(sandbox_path_t *sandbox)
{
	 * still request double fork() avoid change anything on main user_namespace
	 *
	 * need to figure how to share memory between a.out
	 *

	int parent_uid = getuid();
	int parent_gid = getgid();

	printf("getuid before unshare: %d\n", parent_uid);
	printf("getgid before unshare: %d\n", parent_gid);

	unshare(CLONE_NEWUSER);

	printf("getuid after unshare: %d\n", getuid());
	printf("getgid after unshare: %d\n", getgid());

	printf("try parent uid: %d\n", parent_uid);
	printf("try parent gid: %d\n", parent_gid);

	int child_pid = fork();

	if (child_pid < 0) {
		return EXECUTE_UNKNOW;
	}

	if (child_pid == 0) {
		printf("getuid after unshare and fork: %d\n", getuid());
		printf("getgid after unshare and fork: %d\n", getgid());
		_exit(0);
		int uid_fd = open("/proc/self/uid_map", O_RDONLY);
		if (uid_fd < 0) {
			_exit(0);
		}

		char buffer[1024] = {0};
		int uid_map[3];
		int gid_map[3];
		int read_size;

		read_size = read(uid_fd, buffer, sizeof(buffer));
		close(uid_fd);

		if (read_size < 0) {
			_exit(0);
		}

		printf("%s\n", buffer);
	}

	int status;
	status = waitpid(child_pid, &status, 0);

	return 0;
}
*/

/* tmp */
int main()
{
	sandbox_path_t sandbox;
	snprintf(sandbox.base, sizeof(sandbox.base), "/tmp/pj/runtime");

	problem_set_t problem_set;
	problem_set.job_count;
	snprintf(problem_set.base_path, sizeof(problem_set.base_path),
			"%s", "/tmp/pj_pro/0");

	execute_status_t res = execute(&sandbox, &problem_set);
}
/* end tmp */
