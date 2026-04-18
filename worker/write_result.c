/*
 * This function should take response out
 */

#include "../config.c"
#include "../include/global.h"
#include "../include/judge/result.h"
#include "../include/utils/potato_try.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static const char *judge_status_string[] = {
	"ACCEPT",
	"WRONG_ANSWER",
	"COMPILE_ERROR",
	"TIME_LIMIT",
	"MEMORY_LIMIT",
	"SIGSEGV",
	"SIGSYS",
	"NO_SOLUTION",
	"NO_DRIVER",
	"NO_PROBLEM_CONFIG",
	"UNKNOW_ERROR",
};

#define WRITE_FNAME "judge.json"
#define DONE_FNAME "done.json"
#define MAX_CASE_TEXT_COUNT 200
int8_t pj_write_result(const judge_result_t *result,
	const int case_count,
	const char *output_dir)
{
	int8_t ret_err = -1;
	char *buffer;
	int fd = -1;

	if (result == NULL ||
	case_count <= 0 ||
	output_dir == NULL) {
		goto err_out;
	}

	char str_write_path[MAX_PATH_LENGTH];
	char str_done_path[MAX_PATH_LENGTH];
	snprintf(str_write_path, sizeof(str_write_path),
		"%s/%s", output_dir, WRITE_FNAME);
	snprintf(str_done_path, sizeof(str_done_path),
		"%s/%s", output_dir, DONE_FNAME);

	ssize_t bfr_size = case_count * MAX_CASE_TEXT_COUNT;
	buffer = malloc(bfr_size * sizeof(char));
	if (buffer == NULL) goto err_out;

	ssize_t offset = 0;
	ssize_t w = 0;

	/* write json into buffer */
	w = snprintf(buffer+offset, bfr_size-offset, "{\"judge_result\":[");
	if (w < 0 || w >= bfr_size - offset) {
		ret_err = -2; goto err_out;
	}
	for (int i = 0; i < case_count; ++i) {
		const judge_result_t *curr = &result[i];

		w = snprintf(buffer+offset, bfr_size-offset,
			"{\"id\":%d, \"result\":\"%s\", \"cpu\":%lld, \"mem\":%lld},",
			curr->id,
			judge_status_string[curr->status],
			curr->usage.time_us,
			curr->usage.mem_kb);
		if (w < 0 || w >= bfr_size - offset) {
			ret_err = -2; goto err_out;
		}
		offset += w;
	}
	if (case_count > 1) {
		--offset;	// remove trail ","
	}
	w = snprintf(buffer + offset, bfr_size - offset, "]}");
	if (w < 0 || w >= bfr_size - offset) {
		ret_err = -2; goto err_out;
	}
	offset += w;

	TRY_GIVE(open(str_write_path, O_CREAT | O_WRONLY), fd);

	ssize_t written = 0;
	ssize_t exp_len = offset;
	offset = 0;
	while (written < exp_len) {
		TRY_GIVE(write(fd, buffer+offset, exp_len-offset), w);
		written += w;
	}
	close(fd); fd = -1;
	free(buffer); buffer = NULL;

	rename(str_write_path, str_done_path);

	return 0;
err_out:
	if (fd >= 0) {
		close(fd); fd = -1;
	}
	free(buffer);
	return ret_err;
}
