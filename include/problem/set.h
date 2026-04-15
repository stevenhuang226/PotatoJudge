#ifndef PROBLEM_SET_H
#define PROBLEM_SET_H

#include <stdint.h>
#include "./limit.h"

typedef struct {
	char input_path[MAX_PATH_LENGTH];	// input.bin path
	char output_path[MAX_PATH_LENGTH];	// output.bin path
	char checker_path[MAX_PATH_LENGTH];	// checker.out path

	off_t max_result_size;			// expect maximum result size

	problem_limit_t limit;
} problem_set_t;

#endif
