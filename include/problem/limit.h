#ifndef PROBLEM_LIMIT_H
#define PROBLEM_LIMIT_H

#include <stdint.h>

typedef struct {
	int64_t time_s;			// limit times second
	int64_t as_mb;			// limit memory space
	int64_t process;			// limit child process number
	int64_t file_mb;			// limit file_size
	int64_t stack_mb;			// limit stack size
} problem_limit_t;

#endif
