/*
#include "../config.c"
#include "../include/config.h"
#include "../include/sandbox_path.h"
#include "../include/worker/listener.h"
*/

#include "../include/utils/potato_try.h"

#include <sys/socket.h>

int8_t setup_listener()
{
	int8_t ret_err = -1;
	int fd = -1;

	TRY_GIVE(socket(AF_UNIX, SOCK_STREAM, 0), fd);


err_out:
	if (fd >= 0) {
		close(fd);
	}
	return ret_err;
}

int8_t listen_submit()
{
	int8_t ret_err = -1;

err_out:
	return ret_err;
}
