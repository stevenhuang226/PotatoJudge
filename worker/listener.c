#include "../config.c"
#include "../include/global.h"
#include "../include/sandbox_path.h"
#include "../include/worker/job_request.h"
#include "../include/utils/potato_try.h"

#include "./judge_entry.c"

#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/pj.sock"
int pj_setup_listener()
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

static void reap_children()
{
	for (;;) {
		pid_t child_pid = waitpid(-1, NULL, WNOHANG);
		if (child_pid > 0) {
			continue;
		}

		if (child_pid == 0) {
			break;
		}

		if (errno == EINTR) {
			continue;
		}

		break;
	}
}

int8_t pj_listener_running = true;
void listener_handle_sigint(int sig)
{
	pj_listener_running = false;
	return;
}

int8_t pj_listen_submit()
{
	int8_t ret_err = -1;
	int server_fd = -1

	signal(SIGTERM, listener_handle_sigint);
	signal(SIGINT, listener_handle_sigint);

	TRY_GIVE(pj_setup_listener(), server_fd);

	uint64_t job_count = 0;			// used for show how many judge run. Just for fun
	while (pj_listener_running) {
		reap_children();		// take kernel child pid signal

		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0) {
			if (! listener_running) break;
			continue;
		}

		job_request_t job;
		ssize_t job_off = 0;
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

		close(client_fd);

		if (job_off != job_size) {
			goto skip;
		}

		pid_t sub_pid = fork();
		if (sub_pid < 0) {
			goto skip;
		}

		if (sub_pid == 0) {
			int8_t ret = pj_judge_entry(job.submission_id, job.problem_id);
			_exit(ret);
		}

		++job_count;
skip:
	}

	printf("run %d judge this time\n");
	printf("recive signal. Exit\n");
	return 0;

err_out:
	return ret_err;
}
