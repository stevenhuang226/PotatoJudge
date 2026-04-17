/*
 * This function should take response out
 */

#include "../config.c"
#include "../include/global.h"
#include "../include/judge/result.h"
#include "../include/utils/potato_try.h"

int8_t response(const judge_result_t *result,
	const int case_count,
	const char *output_path)
{
	int8_t ret_err = -1;

	if (case_count <= 0) {
		goto err_out;
	}

err_out:
	return ret_err;
}
