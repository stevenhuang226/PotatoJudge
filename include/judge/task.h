#ifndef JUDGE_TASK_H
#define JUDGE_TASK_H

#include <stdint.h>
#include "../compile/types.h"

typedef struct {
	uint32_t submission_id;
	uint32_t problem_id;
	compiler_type_t compiler_type;
} judge_task_t;

#endif
