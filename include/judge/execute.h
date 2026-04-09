#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>

typedef enum {
	EXECUTE_OK,		// accepted
	EXECUTE_WA,		// wrong answer

	EXECUTE_RE,		// runtime error
	EXECUTE_SIGSEGV,	// segment fault (maybe same as runtime error)
	EXECUTE_SIGSYS,
	EXECUTE_SIGABRT,	// abort() called
	EXECUTE_SIGFPE,		// divide by zero, floating error
	EXECUTE_SIGILL,		// illegal instruction
	EXECUTE_SIGKILL,	// killed (by OOM killer or judge)

	EXECUTE_TLE,
	EXECUTE_MLE,

	EXECUTE_SEC,

	EXECUTE_NO_INPUT,	// no input%d.bin
	EXECUTE_NO_OUTPUT,	// no output%d.bin
	EXECUTE_NO_CHECKER,	// no checker.out

	EXECUTE_UNKNOW,
} execute_status_t;

typedef struct {
	uint64_t time_us;
	uint64_t mem_kb;
} execute_resource_t;

#define IS_EXECUTE_STATUS(code) ((code) >= EXECUTE_OK && (code) <= EXECUTE_UNKNOW)

#endif
