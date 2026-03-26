#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>

typedef enum {
	COMPILE_SUCCESSES,
	COMPILE_FAIL,
	COMPILE_SOLUTION_NOT_FOUND,
	COMPILE_DRIVER_NOT_FOUND,
	COMPILE_BUILD_FAIL,
} compile_status_t;

typedef enum {
	COMPILER_GCC,
	COMPILER_GPP,
} compiler_type_t;

#endif
