#include "../config.c"
#include <unistd.h>

int compile_gpp()
{
	return -1;	// CPP doesn't work currently

	int ret_err = -1;

	execl("/usr/bin/gcc",
		"g++",
		"-x", "c++",
		SETLIMIT_NAME,
		DRIVER_NAME,
		SOLUTION_NAME,
		"-static",
		"-o", COMPILED_OUT_NAME,
		NULL);

	return ret_err;
err_out:
	return ret_err;
}
