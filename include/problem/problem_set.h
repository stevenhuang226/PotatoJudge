#ifndef PROBLEM_SET_H
#define PROBLEM_SET_H

typedef struct {
	char base_path[MAX_PATH_LENGTH];

	int job_count;
	int time_limit;
	int memory_limit;
} problem_set_t;

#endif
