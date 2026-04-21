#ifndef JOB_REQUEST_H
#define JOB_REQUEST_H

#include <stdint.h>

typedef struct {
	uint32_t submission_id;
	uint32_t problem_id;
} job_request_t;

#endif
