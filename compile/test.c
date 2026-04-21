#include "../config.c"
#include "../include/global.h"
#include "../include/sandbox_path.h"
#include "../include/utils/potato_try.h"

#include "./compile.c"

int main()
{
	int ret_err = -1;

	sandbox_path_t sandbox_path;
	snprintf(sandbox_path.base, sizeof(sandbox_path.base),
		"%s", "/tmp/pj/runtime");
	snprintf(sandbox_path.usr, sizeof(sandbox_path.usr),
		"%s/usr", sandbox_path.base);
	snprintf(sandbox_path.lib, sizeof(sandbox_path.lib),
		"%s/lib", sandbox_path.base);
	snprintf(sandbox_path.lib64, sizeof(sandbox_path.lib64),
		"%s/lib64", sandbox_path.base);
	snprintf(sandbox_path.dev, sizeof(sandbox_path.dev),
		"%s/dev", sandbox_path.base);
	snprintf(sandbox_path.dev_null, sizeof(sandbox_path.dev_null),
		"%s/null", sandbox_path.dev);

	compile_status_t compile_res = compile(&sandbox_path, COMPILER_GCC);

	if (compile_res == COMPILE_SUCCESS) {
		printf("SUCCESS\n");
		return 0;
	} else {
		printf("FAIL\n");
	}

	switch(compile_res) {
	case COMPILE_FAIL:
		printf("compile_fail\n");
	case COMPILE_NO_SOLUTION:
		printf("no solution\n");
	case COMPILE_NO_DRIVER:
		printf("no driver\n");
	case COMPILE_UNKNOW:
		printf("compile unknow\n");
	}


	return 0;

err_out:
	printf("FAIL\n");
	return ret_err;
}
