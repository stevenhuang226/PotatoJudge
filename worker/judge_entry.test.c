#include "../config.c"
#include "../include/utils/potato_try.h"

#include "./init.c"
#include "./judge_entry.c"

int main()
{
	int ret_err = -1;

	TRY(initialization());

	uint32_t submission_id = 0;
	uint32_t problem_id = 0;
	int8_t ret = pj_judge_entry(submission_id, problem_id);

	if (ret < 0) {
		printf("FAIL\n");
	} else {
		printf("SUCCESS\n");
	}

	return 0;

err_out:
	printf("FAIL\n");
	return -1;
}
