#ifndef JUDGE_CONFIG_H
#define JUDGE_CONFIG_H

#include "./sandbox_path.h"

typedef struct {
	char base_submission[MAX_PATH_LENGTH];
	char base_problem[MAX_PATH_LENGTH];
	char base_workspace[MAX_PATH_LENGTH];
	sandbox_path_t sandbox_path;
} judge_config_t;

#endif
