#include "../include/global.h"
#include "../include/utils/potato_try.h"
#include "../include/sandbox_path.h"
#include "../include/compile/types.h"
#include "../include/compile/status.h"

#include <sys/resource.h>
#include <sys/mount.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>

#include "./use_gcc.c"
#include "./use_gpp.c"

typedef int (*compiler_func)();

static compiler_func type2compiler[COMPILER_COUNT] = {
	[COMPILER_GCC] = compile_gcc,
	[COMPILER_GPP] = compile_gpp,
};

int8_t pj_exec_compiler(const compiler_type_t type)
{
	if (!IS_COMPILER_TYPE(type)) {
		return -1;
	}

	compiler_func func = type2compiler[type];
	if (!func) {
		return -1;
	}
	return func();
}

compile_status_t pj_sandbox_compile(
	const sandbox_path_t *sandbox_path,
	const compiler_type_t compiler_type)
{
	int ret_err = COMPILE_UNKNOW;
	int null_fd = -1;

	if (sandbox_path == NULL ||
	!IS_COMPILER_TYPE(compiler_type)) {
		goto err_out;
	}

	TRY(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL));
	TRY(mount("/usr", sandbox_path->usr, NULL, MS_BIND, NULL));
	TRY(mount(NULL, sandbox_path->usr, NULL,
		MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL));
	TRY(mount("/lib", sandbox_path->lib, NULL, MS_BIND, NULL));
	TRY(mount(NULL, sandbox_path->lib, NULL,
		MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL));
	TRY(mount("/lib64", sandbox_path->lib64, NULL, MS_BIND, NULL));
	TRY(mount(NULL, sandbox_path->lib64, NULL,
		MS_REMOUNT | MS_BIND | MS_RDONLY | MS_NODEV | MS_NOSUID, NULL));
	TRY(mount("/dev/null", sandbox_path->dev_null, NULL, MS_BIND, NULL));
	TRY(mount(NULL, sandbox_path->dev_null, NULL,
		MS_REMOUNT | MS_BIND | MS_NOSUID, NULL));

	TRY(chdir(sandbox_path->base));
	TRY(chroot("."));
	TRY(chdir("/"));

	TRY_GIVE(open("/dev/null", O_WRONLY), null_fd);
	TRY(dup2(null_fd, STDOUT_FILENO));
	TRY(dup2(null_fd, STDERR_FILENO));
	close(null_fd); null_fd = -1;

	int limit_time_s = 2;
	int limit_as_mb = 256;

	struct rlimit cpu = {
		limit_time_s,
		limit_time_s
	};
	struct rlimit as = {
		limit_as_mb * 1024 * 1024,
		limit_as_mb * 1024 * 1024,
	};

	TRY_NOEQU(setrlimit(RLIMIT_CPU, &cpu), 0);
	TRY_NOEQU(setrlimit(RLIMIT_AS, &as), 0);

	TRY(pj_exec_compiler(compiler_type));
err_out:
	if (null_fd >= 0) {
		close(null_fd); null_fd = -1;
	}
	return ret_err;
}
