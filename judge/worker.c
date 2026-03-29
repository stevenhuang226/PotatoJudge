#include "../config.c"
#include "../include/judge/task.h"
#include "../include/judge/status.h"

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

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

int8_t create_sandbox_paths()
{
	snprintf(g_judge_config.sandbox_path.base, sizeof(g_judge_config.sandbox_path.base),
			"%s/runtime",
			g_judge_config.base_workspace);

	if (mkdir(g_judge_config.sandbox_path.base, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path base");
		return -1;
	}

	snprintf(g_judge_config.sandbox_path.usr, sizeof(g_judge_config.sandbox_path.usr),
			"%s/usr",
			g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.usr, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path usr");
		return -1;
	}

	snprintf(g_judge_config.sandbox_path.lib, sizeof(g_judge_config.sandbox_path.lib),
			"%s/lib",
			g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.lib, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path lib");
		return -1;
	}

	snprintf(g_judge_config.sandbox_path.lib64, sizeof(g_judge_config.sandbox_path.lib64),
			"%s/lib64",
			g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.lib64, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path lib64");
		return -1;
	}

	snprintf(g_judge_config.sandbox_path.tmp, sizeof(g_judge_config.sandbox_path.tmp),
			"%s/tmp",
			g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.tmp, 0775) < 0 && errno != EEXIST) {
		perror("create sandbox path tmp");
		return -1;
	}
	chmod(g_judge_config.sandbox_path.tmp, 01777);

	snprintf(g_judge_config.sandbox_path.dev, sizeof(g_judge_config.sandbox_path.dev),
			"%s/dev",
			g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.dev, 0775) < 0 && errno != EEXIST) {
		perror("create sandbox path dev");
		return -1;
	}

	snprintf(g_judge_config.sandbox_path.dev_null, sizeof(g_judge_config.sandbox_path.dev_null),
			"%s/null",
			g_judge_config.sandbox_path.dev);
	int dev_null_fd = open(g_judge_config.sandbox_path.dev_null, O_CREAT | O_RDWR, 0666);
	if (dev_null_fd < 0) {
		perror("create sandbox path dev null");
		return -1;
	}
	close(dev_null_fd);

	return 0;
}

/* entry function */
int initialization()
{
	if (load_config() < 0) {
		return -1;
	}

	if (create_sandbox_paths() < 0) {
		return -1;
	}

	/* loop track sumbission */
}
