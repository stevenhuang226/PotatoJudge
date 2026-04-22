#ifndef JUDGE_STATUS_H
#define JUDGE_STATUS_H

#include <stdio.h>

typedef enum {
	JUDGE_ACCEPT,
	JUDGE_WRONG_ANSWER,
	JUDGE_COMPILE_ERROR,
	JUDGE_TIME_LIMIT,
	JUDGE_MEMORY_LIMIT,
	JUDGE_SIGSEGV,			// segment fail
	JUDGE_SIGSYS,			// use banned syscall
	JUDGE_NO_SOLUTION,
	JUDGE_NO_DRIVER,
	JUDGE_NO_PROBLEM_CONFIG,
	JUDGE_UNKNOW_ERROR,
} judge_status_t;

#define IS_JUDGE_STATUS(code) ((code) >= JUDGE_ACCEPT && (code) <= JUDGE_UNKNOW_ERROR)

#endif
