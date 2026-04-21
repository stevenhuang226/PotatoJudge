#include "./config.c"
#include "./include/global.h"
#include "./include/utils/potato_try.h"

#include "./worker/init.c"
#include "./worker/listener.c"

int main()
{
	int ret_err = -1;

	initialization();
	int server_fd = -1;
	TRY_GIVE(pj_setup_listener(), server_fd);
	pj_listen_submit(server_fd);

	return 0;
err_out:
	return ret_err;
}
