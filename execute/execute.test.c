#include "./execute.c"

#include <stdint.h>

void ptype(execute_status_t stat)
{
	if (stat == EXECUTE_OK) {
		printf("execute ok\n");
	} else if (stat == EXECUTE_WA) {
		printf("execute wa\n");
	} else if (stat == EXECUTE_TLE) {
		printf("execute tle\n");
	} else if (stat == EXECUTE_MLE) {
		printf("execute mlt\n");
	} else if (stat == EXECUTE_UNKNOW) {
		printf("unknow\n");
	} else if (stat == EXECUTE_SIGKILL) {
		printf("sig kill\n");
	} else if (stat == EXECUTE_NO_INPUT) {
		printf("no input\n");
	} else if (stat == EXECUTE_NO_OUTPUT) {
		printf("no output\n");
	} else if (stat == EXECUTE_NO_CHECKER) {
		printf("no checker\n");
	} else if (stat == EXECUTE_SIGILL) {
		printf("sigill\n");
	} else if (stat == EXECUTE_RE) {
		printf("re\n");
	} else if (stat == EXECUTE_SIGSYS) {
		printf("sig sys\n");
	} else {
		printf("other\n");
	}
}

int main()
{
	sandbox_path_t sandbox_path;
	snprintf(sandbox_path.base, sizeof(sandbox_path.base), "/tmp/pj/runtime");

	problem_set_t problem_set;
	snprintf(problem_set.input_path, sizeof(problem_set.input_path),
		"/tmp/pj_pro/0/input0.bin");
	snprintf(problem_set.output_path, sizeof(problem_set.output_path),
		"/tmp/pj_pro/0/output0.bin");
	snprintf(problem_set.checker_path, sizeof(problem_set.checker_path),
		"/tmp/pj_pro/0/checker.out");

	problem_set.limit.time_s = 2;
	problem_set.limit.as_mb = 128;
	problem_set.limit.stack_mb = 8;
	problem_set.limit.file_mb = 64;
	problem_set.limit.process = 4;

	problem_set.max_result_size = 12800;

	execute_resource_t exe_usage;
	execute_status_t exe = execute(&sandbox_path, &problem_set, &exe_usage);

	printf("time_us: %llu, mem_kb: %llu\n", exe_usage.time_us, exe_usage.mem_kb);

	ptype(exe);

	printf("%d\n", exe);

	if (exe == EXECUTE_OK) {
		printf("SUCCES\n");
	} else {
		printf("FAIL\n");
	}

	return 0;
}
