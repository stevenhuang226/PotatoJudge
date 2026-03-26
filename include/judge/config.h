#ifndef JUDGE_CONFIG_H
#define JUDGE_CONFIG_H

typedef struct {
	char base_submission[MAX_PATH_LENGTH];
	char base_problem[MAX_PATH_LENGTH];
	char base_workspace[MAX_PATH_LENGTH];
	char sandbox_path[MAX_PATH_LENGTH];
} judge_config_t;

extern judge_config_t g_judge_config;

#endif
