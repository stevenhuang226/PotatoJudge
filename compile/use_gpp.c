#include <unistd.h>

int compile_gpp()
{
	int ret_err = -1;

	execl("/usr/bin/gcc",
		"g++",
		"solution.cpp",
		"driver.c",
		"-static",
		"-o", "a.out",
		NULL);

	return ret_err;
err_out:
	return ret_err;
}
