#ifndef COMPILE_STATUS_H
#define COMPILE_STATUS_H

#include <stdio.h>

typedef enum {
	COMPILE_SUCCESS = 0,
	COMPILE_FAIL,
	COMPILE_NO_SOLUTION,
	COMPILE_NO_DRIVER,
	COMPILE_UNKNOW = 127,
} compile_status_t;

#define IS_COMPILE_STATUS(code) \
	((code) == COMPILE_SUCCESS || \
	 (code) == COMPILE_FAIL || \
	 (code) == COMPILE_NO_SOLUTION || \
	 (code) == COMPILE_NO_DRIVER || \
	 (code) == COMPILE_UNKNOW)

#endif
