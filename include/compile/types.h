#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include <stdio.h>

typedef enum {
	COMPILER_NULL,
	COMPILER_GCC,
	COMPILER_GPP,
	COMPILER_COUNT,
} compiler_type_t;

#define IS_COMPILER_TYPE(code) ((code) > COMPILER_NULL && (code) < COMPILER_COUNT)

#endif
