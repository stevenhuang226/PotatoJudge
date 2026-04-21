#include "./config.c"
#include "./include/global.h"
#include "./include/utils/potato_try.h"

#include "./worker/init.c"
#include "./worker/listener.c"

int main()
{
	int ret_err = -1;

	TRY(initialization());
	int8_t listen = pj_listen_submit();

	return listen;

err_out:
	return ret_err;
}
