#include "../config.c"
#include "../include/judge/task.h"
#include "../include/judge/status.h"

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

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

int8_t create_sandbox_path()
{
	int permissions = 0755;

	char sandbox_path[MAX_PATH_LENGTH];
	snprintf(sandbox_path, sizeof(sandbox_path),
			"%s/runtime/",
			g_judge_config.base_workspace);

	int mkdir_sandbox = mkdir(sandbox_path, permissions);

	if (mkdir_sandbox != 0) {
		int err = errno;

		if (err != EEXIST) {
			perror("create_sandbox_path: mkdir failed");
			return -1;
		}
	}

	snprintf(g_judge_config.sandbox_path, sizeof(g_judge_config.sandbox_path),
			"%s",
			sandbox_path);

	return 0;
}

/* entry function */
int initialization()
{
	load_config();
	create_sandbox_path();
}
