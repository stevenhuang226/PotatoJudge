/*
 * init system
 * queue for jude system. If any job exists (create by other process). Automatically call run_judge()
 */
#include "./judge_config.h":
#include <stdio.h>
#include <stdint.h>

typedef struct {
	char base_submission[MAX_PATH_LENGTH];
	char base_problem[MAX_PATH_LENGTH];
	char base_workspace[MAX_PATH_LENGTH];
} JudgeConfig;

JudgeConfig JUDGE_CONFIG;

int8_t load_config()
{
	FILE *fp = fopen(CONF_PATH, "r");
	if (! fp) {
		return -1;
	}

	char line[MAX_PATH_LENGTH];

	while (fgets(line, sizeof(line), fp)) {
		sscanf(line, "base_submission=%s", JUDGE_CONFIG.base_submission);
		sscanf(line, "base_problem=%s", JUDGE_CONFIG.base_problem);
		sscanf(line, "base_workspace=%s", JUDGE_CONFIG.base_workspace);
	}

	fclose(fp);
}
