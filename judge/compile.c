#include "../include/judge/compiler.h"
#include "../include/judge/sandbox_path.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

compile_status_t compile_gcc(const sandbox_path_t *sandbox)
{
	pid_t child_pid = fork();

	if (child_pid < 0) return COMPILE_BUILD_FAIL;

	if (child_pid == 0) {

		/* TODO unshare(CLONE_NEWNS) avoid umount problem */

		if(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) < 0) {
			perror("mount private");
			_exit(127);
		}

		if (mount("/usr", sandbox->usr, NULL, MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox->usr, NULL,
					MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL) < 0) {
			perror("mount usr");
			_exit(127);
		}

		if (mount("/lib", sandbox->lib, NULL, MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox->lib, NULL,
					MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL) < 0) {
			perror("mount lib");
			_exit(127);
		}

		if (mount("/lib64", sandbox->lib64, NULL, MS_BIND, NULL) < 0 ||
				mount(NULL, sandbox->lib64, NULL,
					MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL) < 0) {
			perror("mount lib64");
			_exit(127);
		}

		if (mount("tmpfs", sandbox->tmp, "tmpfs", MS_NODEV | MS_NOSUID, "size=64M") < 0) {
			perror("mount tmpfs");
			_exit(127);
		}

		if (mount("/dev/null", sandbox->dev_null, NULL, MS_BIND, NULL) < 0) {
			perror("mount dev null");
			_exit(127);
		}

		if (chroot(sandbox->base) < 0) {
			perror("chroot");
			_exit(127);
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

		execl("/usr/bin/gcc",
				"gcc",
				"solution.c",
				"driver.c",
				"-o", "a.out",
				NULL);

		perror("run gcc error");
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

