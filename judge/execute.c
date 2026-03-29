#include "../config.c"
#include "../include/judge/execute.h"
#include "../include/judge/sandbox_path.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

execute_status_t execute(sandbox_path_t *sandbox)
{
}

execute_status_t execute(sandbox_path_t *sandbox)
{
	/*
	 * still request double fork() avoid change anything on main user_namespace
	 *
	 * need to figure how to share memory between a.out
	 *
	 */

	int parent_uid = getuid();
	int parent_gid = getgid();

	printf("getuid before unshare: %d\n", parent_uid);
	printf("getgid before unshare: %d\n", parent_gid);

	unshare(CLONE_NEWUSER);

	printf("getuid after unshare: %d\n", getuid());
	printf("getgid after unshare: %d\n", getgid());

	printf("try parent uid: %d\n", parent_uid);
	printf("try parent gid: %d\n", parent_gid);

	int child_pid = fork();

	if (child_pid < 0) {
		return EXECUTE_UNKNOW;
	}

	if (child_pid == 0) {
		printf("getuid after unshare and fork: %d\n", getuid());
		printf("getgid after unshare and fork: %d\n", getgid());
		_exit(0);
		/*
		int uid_fd = open("/proc/self/uid_map", O_RDONLY);
		if (uid_fd < 0) {
			_exit(0);
		}

		char buffer[1024] = {0};
		int uid_map[3];
		int gid_map[3];
		int read_size;

		read_size = read(uid_fd, buffer, sizeof(buffer));
		close(uid_fd);

		if (read_size < 0) {
			_exit(0);
		}

		printf("%s\n", buffer);
		*/
	}

	int status;
	status = waitpid(child_pid, &status, 0);

	return 0;
}

/* tmp */
int main()
{
	sandbox_path_t sandbox;
	snprintf(sandbox.base, sizeof(sandbox.base), "/tmp/pj/runtime");

	execute_status_t res = execute(&sandbox);
}
/* end tmp */
