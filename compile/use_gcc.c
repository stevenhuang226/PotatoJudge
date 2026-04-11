#include <unistd.h>

int compile_gcc()
{
	int ret_err = -1;

	execl("/usr/bin/gcc",
		"gcc",
		"solution.c",
		"driver.c",
		"-static",
		"-o", "a.out",
		NULL);

	return ret_err;
err_out:
	return ret_err;
}
