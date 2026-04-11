#include "../include/utils/potato_try.h"
#include "../include/problem/limit.h"

#include <sys/resource.h>

static inline int8_t set_limit(problem_limit_t *limit)
{
	struct rlimit cpu = {limit->time_s, limit->time_s};
	struct rlimit process = {limit->process, limit->process};
	struct rlimit as =
		{limit->as_mb * 1024 * 1024, limit->as_mb * 1024 * 1024};
	struct rlimit stack =
		{limit->stack_mb * 1024 * 1024, limit->stack_mb * 1024 * 1024};
	struct rlimit fsize =
		{limit->file_mb * 1024 * 1024, limit->file_mb * 1024 * 1024};

	TRY_NOEQU(setrlimit(RLIMIT_CPU, &cpu), 0);
	TRY_NOEQU(setrlimit(RLIMIT_NPROC, &process), 0);
	TRY_NOEQU(setrlimit(RLIMIT_AS, &as), 0);
	TRY_NOEQU(setrlimit(RLIMIT_FSIZE, &fsize), 0);
	TRY_NOEQU(setrlimit(RLIMIT_STACK, &stack), 0);

	return 0;
err_out:
	return -1;
}
static inline int8_t set_seccomp()
{
	// TODO filter setting
	return 0;
}
