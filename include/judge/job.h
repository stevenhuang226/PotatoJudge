#ifndef JOB_T
#define JOB_T

#include "./execute.h"
#include <stdint.h>

typedef struct {
	uint32_t magic; // header check

	int job_id;
	execute_status_t state;

	ssize_t input_size;
	ssize_t exp_output_size;

	char data[];
} job_shm_t;

typedef struct {
	job_shm_t *ptr;
	ssize_t size;
} job_entry_t;

#endif
