#ifndef EXECUTE_H
#define EXECUTE_H

typedef enum {
	EXECUTE_OK,		// accepted
	EXECUTE_WA,		// wrong answer

	EXECUTE_RE,		// runtime error
	EXECUTE_SIGSEGV,	// segment fault
	EXECUTE_SIGABRT,	// abort() called
	EXECUTE_SIGFPE,	// divide by zero, floating error
	EXECUTE_SIGILL,	// illegal instruction
	EXECUTE_SIGKILL,	// killed (by OOM killer or judge)

	EXECUTE_TLE,
	EXECUTE_MLE,

	EXECUTE_SEC,

	EXECUTE_NO_INPUT,	// no input%d.bin
	EXECUTE_NO_OUTPUT,	// no output%d.bin
	EXECUTE_NO_CHECKER,	// no checker.out

	EXECUTE_UNKNOW,
} execute_status_t;

#endif
