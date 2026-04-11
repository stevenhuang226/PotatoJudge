#include "../config.c"
#include "../include/judge/task.h"
#include "../include/judge/status.h"
#include "../include/utils/potato_try.h"

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

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
	int8_t ret_err = -1;

	snprintf(g_judge_config.sandbox_path.base, sizeof(g_judge_config.sandbox_path.base),
		"%s/runtime", g_judge_config.base_workspace);
	if (mkdir(g_judge_config.sandbox_path.base, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path base");
		goto err_out;
	}

	snprintf(g_judge_config.sandbox_path.usr, sizeof(g_judge_config.sandbox_path.usr),
		"%s/usr", g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.usr, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path usr");
		goto err_out;
	}

	snprintf(g_judge_config.sandbox_path.lib, sizeof(g_judge_config.sandbox_path.lib),
		"%s/lib", g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.lib, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path lib");
		goto err_out;
	}

	snprintf(g_judge_config.sandbox_path.lib64, sizeof(g_judge_config.sandbox_path.lib64),
		"%s/lib64", g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.lib64, 0775) != 0 && errno != EEXIST) {
		perror("create sandbox path lib64");
		goto err_out;
	}

	snprintf(g_judge_config.sandbox_path.tmp, sizeof(g_judge_config.sandbox_path.tmp),
		"%s/tmp", g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.tmp, 0775) < 0 && errno != EEXIST) {
		perror("create sandbox path tmp");
		goto err_out;
	}
	chmod(g_judge_config.sandbox_path.tmp, 01777); // mkdir cannot set 01777, request another chmod

	snprintf(g_judge_config.sandbox_path.dev, sizeof(g_judge_config.sandbox_path.dev),
		"%s/dev", g_judge_config.sandbox_path.base);
	if (mkdir(g_judge_config.sandbox_path.dev, 0775) < 0 && errno != EEXIST) {
		perror("create sandbox path dev");
		goto err_out;
	}

	snprintf(g_judge_config.sandbox_path.dev_null, sizeof(g_judge_config.sandbox_path.dev_null),
		"%s/null", g_judge_config.sandbox_path.dev);
	int dev_null_fd = open(g_judge_config.sandbox_path.dev_null, O_CREAT | O_RDWR, 0666);
	if (dev_null_fd < 0) {
		perror("create sandbox path dev null");
		goto err_out;
	}
	close(dev_null_fd);

	return 0;

err_out:
	return ret_err;
}

/* init function */
int initialization()
{
	if (load_config() < 0) {
		return -1;
	}

	if (create_sandbox_paths() < 0) {
		return -1;
	}

	/* loop track sumbission */
	return 0;
}

/*
int main()
{
}
*/
