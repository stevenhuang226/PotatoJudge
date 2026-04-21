#include "../include/global.h"
#include "../include/utils/potato_try.h"
#include "../include/sandbox_path.h"
#include "../include/compile/status.h"
#include "../include/compile/types.h"

#include "./sandbox.c"

#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sched.h>
#include <fcntl.h>

compile_status_t pj_compile_isolate(
	const sandbox_path_t *sandbox_path,
	const compiler_type_t compiler_type)
{
	compile_status_t ret_err = COMPILE_UNKNOW;

	if (sandbox_path == NULL ||
	!IS_COMPILER_TYPE(compiler_type)) {
		goto err_out;
	}

	char str_setgroups[] = "deny\n";
	char str_uidmap[32];
	char str_gidmap[32];
	int fd = -1;

	snprintf(str_uidmap, sizeof(str_uidmap), "0 %u 1\n", getuid());
	snprintf(str_gidmap, sizeof(str_gidmap), "0 %u 1\n", getgid());

	TRY(unshare(CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID));

	TRY_GIVE(open("/proc/self/setgroups", O_WRONLY), fd);
	TRY(write(fd, str_setgroups, strlen(str_setgroups)));
	close(fd);

	TRY_GIVE(open("/proc/self/uid_map", O_WRONLY), fd);
	TRY(write(fd, str_uidmap, strlen(str_uidmap)));
	close(fd);

	TRY_GIVE(open("/proc/self/gid_map", O_WRONLY), fd);
	TRY(write(fd, str_gidmap, strlen(str_gidmap)));
	close(fd);

	TRY(setreuid(0, 0)); TRY(setregid(0, 0));

	pid_t sandbox_pid = fork();

	if (sandbox_pid < 0) {
		goto err_out;
	}

	if (sandbox_pid == 0) {
		int ret = pj_sandbox_compile(sandbox_path, compiler_type);
		_exit(ret);
	}

	int status;
	TRY(waitpid(sandbox_pid, &status, 0));
	if (!WIFEXITED(status)) goto err_out;

	int code = WEXITSTATUS(status);

	if (code == 0) {			// exit code == 0, compiler exit 0, compile success
		return COMPILE_SUCCESS;
	} else if (code == COMPILE_UNKNOW) {	// exit code == unknow, not compiler fail, return unknow
		goto err_out;
	}
	return COMPILE_FAIL;			// other code => compiler run, but error, return compile fail

err_out:
	if (fd >= 0) {
		close(fd); fd = -1;
	}

	return ret_err;
}

