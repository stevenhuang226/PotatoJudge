#ifndef JOB_T
#define JOB_T

#include "./execute.h"
#include <stdint.h>

typedef struct {
	uint32_t magic; // check if reading correct memory or not

	int job_id;
	execute_status_t state;

	ssize_t input_size;
	ssize_t exp_output_size;

	char data[];
} job_shm_t;

typedef struct {
	int count;
	job_shm_t *jobs[];
} job_queue_t;

#endif
