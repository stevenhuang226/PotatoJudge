#include "../include/global.h"
#include "../include/utils/potato_try.h"
#include "../include/sandbox_path.h"
#include "../include/execute/status.h"
#include "../include/problem/limit.h"

#include "./set_limit.c"

#include <sys/mount.h>
#include <unistd.h>
#include <sched.h>

#define COMPILED_USER_CODE_NAME "./a.out"
execute_status_t pj_execute_judge(
	sandbox_path_t *sandbox_path,
	problem_limit_t *problem_limit,
	int shm_fd, off_t shm_size)
{
	execute_status_t ret_err = EXECUTE_UNKNOW;
	int null_fd = -1;

	if (sandbox_path == NULL ||
		problem_limit == NULL ||
		shm_fd < 0 || shm_size < 0) {
		goto err_out;
	}

	char str_shm_fd[32];
	char str_shm_size[32];
	snprintf(str_shm_fd, sizeof(str_shm_fd), "%d", shm_fd);
	snprintf(str_shm_size, sizeof(str_shm_size), "%lld", shm_size);

	TRY(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL));
	TRY(mount("/dev/null", sandbox_path->dev_null, NULL, MS_BIND, NULL));
	TRY(mount(NULL, sandbox_path->dev_null, NULL,
		MS_REMOUNT | MS_BIND | MS_NOSUID, NULL));

	TRY(chdir(sandbox_path->base) < 0);
	TRY(chroot("./"));
	TRY(chdir("/"));

	TRY_GIVE(open("/dev/null", O_WRONLY), null_fd);
	TRY(dup2(null_fd, STDOUT_FILENO));
	TRY(dup2(null_fd, STDERR_FILENO));
	close(null_fd); null_fd = -1;

	TRY(set_limit(problem_limit));
	TRY(set_seccomp());

	char *argv[] = {
		COMPILED_USER_CODE_NAME,
		str_shm_fd,
		str_shm_size,
		NULL,
	};

	execv(COMPILED_USER_CODE_NAME, argv);

	return EXECUTE_UNKNOW;
err_out:
	if (shm_fd >= 0) {
		close(shm_fd); shm_fd = -1;
	}
	if (null_fd >= 0) {
		close(null_fd); null_fd = -1;
	}
	return ret_err;
}
