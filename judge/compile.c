#include "../config.c"
#include "../include/judge/compiler.h"
#include "../include/judge/sandbox_path.h"

#include <sys/resource.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

compile_status_t compile_gcc(const sandbox_path_t *sandbox_path)
{
	pid_t supervisor_pid = fork();

	if (supervisor_pid < 0) return COMPILE_BUILD_FAIL;

	if (supervisor_pid != 0) {
		int status;
		if (waitpid(supervisor_pid, &status, 0) < 0) return COMPILE_BUILD_FAIL;
		if (! WIFEXITED(status)) return COMPILE_BUILD_FAIL;

		int code = WEXITSTATUS(status);
		if (code == 0) return COMPILE_SUCCESS;
		if (code == 127) return COMPILE_BUILD_FAIL;
		return COMPILE_FAIL;
	}

	uint32_t host_uid = getuid();
	uint32_t host_gid = getgid();

	if (unshare(CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID |
		CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNET) < 0) {
		perror("unshare");
		_exit(127);
	}

	int sandbox_pid = fork(); // double fork

	if (sandbox_pid != 0) {
		int status;
		if (waitpid(sandbox_pid, &status, 0) < 0) _exit(127);
		if (! WIFEXITED(status)) _exit(127);
		_exit(WEXITSTATUS(status));
	}

	char uid_map[64];
	char gid_map[64];
	char setgroups[] = "deny\n";
	snprintf(uid_map, sizeof(uid_map), "0 %u 1\n", host_uid);
	snprintf(gid_map, sizeof(gid_map), "0 %u 1\n", host_gid);

	int fd;

	fd = open("/proc/self/setgroups", O_WRONLY);
	if (fd < 0 || write(fd, setgroups, strlen(setgroups)) < 0) {
		perror("open setgroups");
		_exit(127);
	}
	close(fd);

	fd = open("/proc/self/uid_map", O_WRONLY);
	if (fd < 0 || write(fd, uid_map, strlen(uid_map)) < 0) {
		perror("open uid_map");
		_exit(127);
	}
	close(fd);

	fd = open("/proc/self/gid_map", O_WRONLY);
	if (fd < 0 || write(fd, gid_map, strlen(gid_map)) < 0) {
		perror("open gid_map");
		_exit(127);
	}
	close(fd);

	if (setreuid(0,0) < 0 || setregid(0,0) < 0) {
		perror("setreuid/setregid");
		_exit(127);
	}

	if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) < 0) {
		perror("mount private");
		_exit(127);
	}
	if (mount("/usr", sandbox_path->usr, NULL, MS_BIND, NULL) < 0 ||
		mount(NULL, sandbox_path->usr, NULL, MS_REMOUNT | MS_BIND | MS_RDONLY |
		MS_NODEV | MS_NOSUID, NULL) < 0) {
		perror("mount usr");
		_exit(127);
	}
	if (mount("/lib", sandbox_path->lib, NULL, MS_BIND, NULL) < 0 ||
		mount(NULL, sandbox_path->lib, NULL, MS_REMOUNT | MS_BIND | MS_RDONLY |
		MS_NODEV | MS_NOSUID, NULL) < 0) {
		perror("mount lib");
		_exit(127);
	}
	if (mount("/lib64", sandbox_path->lib64, NULL, MS_BIND, NULL) < 0 ||
		mount(NULL, sandbox_path->lib64, NULL, MS_REMOUNT | MS_BIND | MS_RDONLY |
		MS_NODEV | MS_NOSUID, NULL) < 0) {
		perror("mount lib64");
		_exit(127);
	}
	if (mount("/dev/null", sandbox_path->dev_null, NULL, MS_BIND, NULL) < 0) {
		perror("mount dev/null");
		_exit(127);
	}

	if (chroot(sandbox_path->base) < 0) {
		perror("chroot");
		_exit(127);
	}

	if (chdir("/") < 0) {
		perror("chdir /");
		_exit(127);
	}

	struct rlimit cpu_limit = {2,2};
	if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
		perror("setrlimit cpu");
		_exit(127);
	}

	struct rlimit mem_limit;
	mem_limit.rlim_cur = 256 * 1024 * 1024; // 256MB
	mem_limit.rlim_max = 256 * 1024 * 1024;
	if (setrlimit(RLIMIT_AS, &mem_limit) != 0) {
		perror("setrlimit memory");
		_exit(127);
	}

	execl("/usr/bin/gcc",
		"gcc",
		"solution.c",
		"driver.c",
		"-o", "a.out",
		NULL);

	_exit(127);
}
