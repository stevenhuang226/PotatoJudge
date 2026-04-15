#include "../include/utils/potato_try.h"
#include "../include/problem/limit.h"

#include <linux/seccomp.h>
#include <linux/filter.h>
#include <sys/resource.h>
#include <linux/prctl.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <stddef.h>

static inline int8_t set_limit(problem_limit_t *limit)
{
	int8_t ret_err = -1;

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
	return ret_err;
}

#define SC_ALLOW(syscall_nr) \
	BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, syscall_nr, 0, 1), \
	BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
static inline int8_t set_seccomp()
{
	return 0; // debug ignore this function
	int8_t ret_err = -1;

	struct sock_filter filter[] = {
		BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, nr)),

		SC_ALLOW(SYS_read),
		SC_ALLOW(SYS_write),
		SC_ALLOW(SYS_exit),
		SC_ALLOW(SYS_exit_group),
		SC_ALLOW(SYS_access),
		SC_ALLOW(SYS_openat),
		SC_ALLOW(SYS_brk),
		SC_ALLOW(SYS_mmap),
		SC_ALLOW(SYS_munmap),
		SC_ALLOW(SYS_fstat),
		SC_ALLOW(SYS_close),
		SC_ALLOW(SYS_execve),
		SC_ALLOW(SYS_mprotect),
		SC_ALLOW(SYS_madvise),
		SC_ALLOW(SYS_futex),
		SC_ALLOW(SYS_clock_gettime),
		SC_ALLOW(SYS_prlimit64),
		SC_ALLOW(SYS_arch_prctl),
		SC_ALLOW(SYS_set_tid_address),
		SC_ALLOW(SYS_set_robust_list),
		SC_ALLOW(SYS_rt_sigaction),
		SC_ALLOW(SYS_rt_sigprocmask),
		SC_ALLOW(SYS_rt_sigreturn),

		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
	};

	struct sock_fprog prog = {
		.len = (uint16_t)(sizeof(filter) / sizeof(filter[0])),
		.filter = filter,
	};

	TRY(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0));
	TRY(syscall(SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &prog));

	return 0;

err_out:
	return ret_err;
}
