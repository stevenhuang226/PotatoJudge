/*
 * init system
 * queue for jude system. If any job exists (create by other process). Automatically call run_judge()
 */
#include "../config.c"
#include "../include/judge/task.h"
#include "../include/judge/status.h"

#include <stdio.h>
#include <stdint.h>

/* initialization config(s) */
int8_t load_config()
{
	FILE *fp = fopen(CONF_PATH, "r");
	if (! fp) {
		return -1;
	}

	char line[MAX_PATH_LENGTH];

	while (fgets(line, sizeof(line), fp)) {
		sscanf(line, "base_submission=%s", g_judge_config.base_submission);
		sscanf(line, "base_problem=%s", g_judge_config.base_problem);
		sscanf(line, "base_workspace=%s", g_judge_config.base_workspace);
	}

	fclose(fp);
}
