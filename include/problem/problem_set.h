#ifndef PROBLEM_H
#define PROBLEM_H

#include <stdint.h>

typedef struct {
	int time_s;			// limit times second
	int as_mb;			// limit memory space
	int process;			// limit child process number
	int file_mb;			// limit file_size
	int stack_mb;			// limit stack size
} problem_limit_t;

typedef struct {
	char input_path[MAX_PATH_LENGTH];	// input.bin path
	char output_path[MAX_PATH_LENGTH];	// output.bin path
	char checker_path[MAX_PATH_LENGTH];	// checker.out path

	int max_result_size;			// expect maximum result size

	problem_limit_t limit;
} problem_set_t;

typedef struct {
	char base_path[MAX_PATH_LENGTH];
	int job_count;
} problem_path_t;

#endif
