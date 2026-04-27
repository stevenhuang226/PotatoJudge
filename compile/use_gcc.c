#include "../config.c"
#include <unistd.h>

int compile_gcc()
{
	int ret_err = -1;

	execl("/usr/bin/gcc",
		"gcc",
		SOLUTION_NAME,
		DRIVER_NAME,
		"-static",
		"-o", COMPILED_OUT_NAME,
		NULL);

	return ret_err;
err_out:
	return ret_err;
}
