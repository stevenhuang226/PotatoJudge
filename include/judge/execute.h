#ifndef EXECUTE_h
#define EXECUTE_h

typedef enum {
	EXECUTE_OK,		// accepted

	EXECUTE_RE,		// runtime error
	EXECUTE_SIGSEGV,	// segment fault
	EXECUTE_SIGABRT,	// abort() called
	EXECUTE_SIGFPE,	// divide by zero, floating error
	EXECUTE_SIGILL,	// illegal instruction
	EXECUTE_SIGKILL,	// killed (by OOM killer or judge)

	EXECUTE_TLE,
	EXECUTE_MLE,

	EXECUTE_SEC,

	EXECUTE_UNKNOW,
} execute_status_t;

#endif
