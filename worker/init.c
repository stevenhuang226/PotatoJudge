#include "../config.c"
#include "../include/global.h"
#include "../include/utils/potato_try.h"

#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

int8_t create_sandbox()
{
	int8_t ret_err = -1;

	snprintf(g_judge_config.sandbox_path.base, sizeof(g_judge_config.sandbox_path.base),
		"%s/runtime", g_judge_config.base_workspace);
	snprintf(g_judge_config.sandbox_path.usr, sizeof(g_judge_config.sandbox_path.usr),
		"%s/usr", g_judge_config.sandbox_path.base);
	snprintf(g_judge_config.sandbox_path.lib, sizeof(g_judge_config.sandbox_path.lib),
		"%s/lib", g_judge_config.sandbox_path.base);
	snprintf(g_judge_config.sandbox_path.lib64, sizeof(g_judge_config.sandbox_path.lib64),
		"%s/lib64", g_judge_config.sandbox_path.base);
	snprintf(g_judge_config.sandbox_path.tmp, sizeof(g_judge_config.sandbox_path.tmp),
		"%s/tmp", g_judge_config.sandbox_path.base);
	snprintf(g_judge_config.sandbox_path.dev, sizeof(g_judge_config.sandbox_path.dev),
		"%s/dev", g_judge_config.sandbox_path.base);
	snprintf(g_judge_config.sandbox_path.dev_null, sizeof(g_judge_config.sandbox_path.dev_null),
		"%s/null", g_judge_config.sandbox_path.dev);

	if (mkdir(g_judge_config.sandbox_path.base, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}

	if (mkdir(g_judge_config.sandbox_path.usr, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}

	if (mkdir(g_judge_config.sandbox_path.lib, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}

	if (mkdir(g_judge_config.sandbox_path.lib64, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}

	if (mkdir(g_judge_config.sandbox_path.tmp, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}
	chmod(g_judge_config.sandbox_path.tmp, 01777);

	if (mkdir(g_judge_config.sandbox_path.dev, 0775) != 0 && errno != EEXIST) {
		perror("mkdir");
		goto err_out;
	}

	int fd = -1;
	TRY_GIVE(open(g_judge_config.sandbox_path.dev_null, O_CREAT | O_RDWR, 0666), fd);
	close(fd);

	return 0;

err_out:
	if (fd >= 0) {
		close(fd); fd = -1;
	}
	return ret_err;
}

int initialization()
{
	int ret_err = -1;

	TRY(load_config());
	TRY(create_sandbox());

	return 0;
err_out:
	return ret_err;
}
