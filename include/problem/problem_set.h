#ifndef PROBLEM_H
#define PROBLEM_H

#include <stdint.h>

typedef struct {
	char input_path[MAX_PATH_LENGTH];
	char output_path[MAX_PATH_LENGTH];
	char checker_path[MAX_PATH_LENGTH];

	int time_s_limit;
	int memory_mb_limit;
} problem_set_t;

typedef struct {
	char base_path[MAX_PATH_LENGTH];
	int job_count;
} problem_path_t;

#endif
