#ifndef JUDGE_RESULT_H
#define JUDGE_RESULT_H

#include "./status.h"
#include "../execute/resource.h"

typedef struct {
	int id;
	judge_status_t status;
	execute_resource_t usage;	
} judge_result_t;

#endif
