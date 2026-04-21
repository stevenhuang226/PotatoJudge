#include "../include/global.h"
#include "../include/sandbox_path.h"
#include "../include/utils/potato_try.h"
#include "../include/compile/status.h"
#include "../include/compile/types.h"

#include "./isolate.c"

#include <sys/wait.h>

compile_status_t pj_compile_entry(const sandbox_path_t *sandbox_path, const compiler_type_t compiler_type)
{
	compile_status_t ret_err = COMPILE_UNKNOW;

	if (sandbox_path == NULL ||
	!IS_COMPILER_TYPE(compiler_type)) {
		goto err_out;
	}

	pid_t parent_pid = fork();

	if (parent_pid < 0) {
		goto err_out;
	}

	if (parent_pid == 0) {
		int ret = pj_compile_isolate(sandbox_path, compiler_type);
		_exit(ret);
	}

	int status;
	TRY(waitpid(parent_pid, &status, 0));
	if (!WIFEXITED(status)) goto err_out;

	int code = WEXITSTATUS(status);

	if (IS_COMPILE_STATUS(code)) {
		return code;
	}

err_out:
	return ret_err;
}
