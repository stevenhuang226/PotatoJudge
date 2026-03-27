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
		perror("load config faild");
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
			"%s/runtime",
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

int8_t create_sandbox_child_path()
{
	int premissions = 0775;

	char sandbox_path_usr[MAX_PATH_LENGTH];
	char sandbox_path_lib[MAX_PATH_LENGTH];
	char sandbox_path_lib64[MAX_PATH_LENGTH];
	char sandbox_path_tmp[MAX_PATH_LENGTH];
	char sandbox_path_dev_null[MAX_PATH_LENGTH];

	snprintf(sandbox_path_usr, sizeof(sandbox_path_usr),
			"%s/usr",
			g_judge_config.sandbox_path);
	snprintf(sandbox_path_lib, sizeof(sandbox_path_lib),
			"%s/lib",
			g_judge_config.sandbox_path);
	snprintf(sandbox_path_lib64, sizeof(sandbox_path_lib64),
			"%s/lib64",
			g_judge_config.sandbox_path);
	snprintf(sandbox_path_tmp, sizeof(sandbox_path_usr),
			"%s/tmp",
			g_judge_config.sandbox_path);
	snprintf(sandbox_path_dev_null, sizeof(sandbox_path_dev_null),
			"%s/dev/null",
			g_judge_config.sandbox_path);

	mkdir(sandbox_path_usr);
	mkdir(sandbox_path_lib);
	mkdir(sandbox_path_lib64);
	mkdir(sandbox_path_tmp);
	mkdir(sandbox_path_dev_null);

	return 0;
}

/* entry function */
int initialization()
{
	if (load_config() < 0) {
		return -1;
	}
	if (create_sandbox_path() < 0) {
		return -1;
	}
	if (create_sandbox_child_path() < 0) {
		return -1;
	}

	/* loop track sumbission */
}
