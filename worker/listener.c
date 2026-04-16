#include <sys/socket.h>
#include "../config.c"
#include "../include/config.h"
#include "../include/sandbox_path.h"
#include "../include/worker/job_request.h"
#include "../include/utils/potato_try.h"

#include <unistd.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/pj.sock"
int setup_listener()
{
	int ret_err = -1;
	int fd = -1;

	TRY_GIVE(socket(AF_UNIX, SOCK_STREAM, 0), fd);

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	unlink(SOCKET_PATH); // delete file if exists

	TRY(bind(fd, (struct sockaddr *)&addr, sizeof(addr)));
	TRY(listen(fd, 16));

	return fd;
err_out:
	if (fd >= 0) {
		close(fd);
	}
	return ret_err;
}

int8_t listener_running = true;

void handle_sigint(int sig)
{
	listener_running = false;
	return;
}

int8_t listen_submit()
{
	int8_t ret_err = -1;
	int server_fd = -1;

	int job_count = 0;			// used for show how many judge run. Just for fun

	TRY_GIVE(setup_listener(), server_fd);

	for (;;) {
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0) {
			if (! listener_running) break;
			continue;
		}

		job_request_t job;
		ssize_t job_off;
		ssize_t job_size = (ssize_t)sizeof(job);

		while (job_off < job_size) {
			ssize_t rs = read(client_fd,
				(void *)&job + job_off,
				job_size - job_off);

			if (rs <= 0) {
				break;
			}
			job_off += rs;
		}

		if (job_off != job_size) {
			goto skip;
		}

		/* create task to submission */
skip:
		close(client_fd);
	}

	printf("run %d judge this time\n");
	printf("recive signal. Exit\n");
	return 0;

err_out:
	return ret_err;
}
