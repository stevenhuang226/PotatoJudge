#include "../config.c"
#include "../utils/copy_fd.c"

#include "../include/judge/execute.h"
#include "../include/problem/problem_set.h"

#include <sys/syscall.h>
#include <sys/wait.h>
#include <linux/memfd.h>
#include <unistd.h>
#include <fcntl.h>

int8_t is_execute_status(int8_t code)
{
	if (code >= EXECUTE_OK && code <= EXECUTE_UNKNOW) {
		return true;
	} else {
		return false;
	}
}

static int memfd_create_wrap(const char *name, const off_t limit)
{
	int fd = syscall(SYS_memfd_create, name, MFD_ALLOW_SEALING);
	if (fd < 0) {
		return -1;
	}

	if (ftruncate(fd, limit) != 0) {
		close(fd);
		perror("ftruncate");
		return -1;
	}

	if (fcntl(fd, F_ADD_SEALS, F_SEAL_GROW) != 0) {
		close(fd);
		perror("F_SEAL_GROW");
		return -1;
	}

	return fd;
}

execute_status_t execute_sandbox(sandbox_path_t *sandbox_path,
	problem_set_t *problem, const int fd_shm, const off_t shm_size)
{
	pid_t sandbox_pid = fork();

	if (sandbox_pid < 0) return EXECUTE_UNKNOW;

	if (sandbox_pid != 0) {
		close(fd_shm);
		int status;
		if (waitpid(sandbox_pid, &status, 0) < 0 ||
			!WIFEXITED(status)) {
			return EXECUTE_UNKNOW;
		}

		return EXECUTE_OK;
	}

	char str_fd_shm[32];
	char str_shm_size[32];
	snprintf(str_fd_shm, sizeof(str_fd_shm), "%d", fd_shm);
	snprintf(str_shm_size, sizeof(str_shm_size), "%d", shm_size);

	char *argv[] = {
		"/tmp/pj/runtime/a.out",
		str_fd_shm,
		str_shm_size,
		NULL
	};

	execv("/tmp/pj/runtime/a.out", argv);

	_exit(EXECUTE_UNKNOW);
}

execute_status_t execute(sandbox_path_t *sandbox_path, problem_set_t *problem_set)
{
	char buffer[4096];
	ssize_t read_size;
	ssize_t write_size;

	int fd_input_file = -1;
	int fd_shm = -1;
	int fd_output_file = -1;
	int fd_exp = -1;

	execute_status_t ret = EXECUTE_UNKNOW;

	fd_input_file = open(problem_set->input_path, O_RDONLY);
	if (fd_input_file < 0) {
		ret = EXECUTE_NO_INPUT;
		goto err_out;
	}

	off_t input_size = lseek(fd_input_file, 0, SEEK_END);
	if (input_size < 0) {
		goto err_out;
	}

	lseek(fd_input_file, 0, SEEK_SET);

	fd_shm = memfd_create_wrap("pj_shm", input_size + 32);
	if (fd_shm < 0) {
		goto err_out;
	}

	if (copy_fd(fd_input_file, fd_shm) != COPY_FD_SUCCESS) {
		goto err_out;
	}

	close(fd_input_file);
	fd_input_file = -1;

	pid_t supervisor_pid = fork();

	if (supervisor_pid < 0) {
		goto err_out;
	}

	if (supervisor_pid == 0) {
		execute_status_t ret = execute_sandbox(sandbox_path, problem_set, fd_shm, input_size);
		close(fd_shm);
		_exit(ret);
	}

	int supervisor_status;
	if (waitpid(supervisor_pid, &supervisor_status, 0) < 0 ||
		!WIFEXITED(supervisor_status)) {
		goto err_out;
	}

	execute_status_t supervisor_code = WEXITSTATUS(supervisor_status);

	if (supervisor_code != EXECUTE_OK) {
		if (is_execute_status(supervisor_code)) {
			ret = supervisor_code;
		}
		goto err_out;
	}

	fd_output_file = open(problem_set->output_path, O_RDONLY);
	if (fd_output_file < 0) {
		ret = EXECUTE_NO_OUTPUT;
		goto err_out;
	}

	off_t output_size = lseek(fd_output_file, 0, SEEK_END);
	if (output_size < 0) {
		goto err_out;
	}

	lseek(fd_output_file, 0, SEEK_SET);

	fd_exp = memfd_create_wrap("pj_exp", output_size + 32);
	if (fd_exp < 0) {
		goto err_out;
	}

	if (copy_fd(fd_output_file, fd_exp) != COPY_FD_SUCCESS) {
		goto err_out;
	}

	close(fd_output_file); fd_output_file = -1;

	char str_fd_shm[32];
	char str_fd_exp[32];
	char str_input_size[32];
	char str_output_size[32];
	snprintf(str_fd_shm, sizeof(str_fd_shm), "%d", fd_shm);
	snprintf(str_fd_exp, sizeof(str_fd_exp), "%d", fd_exp);
	snprintf(str_input_size, sizeof(str_input_size), "%d", input_size);
	snprintf(str_output_size, sizeof(str_output_size), "%d", output_size);

	char *argv[] = {
		problem_set->checker_path,
		str_fd_shm,
		str_fd_exp,
		str_input_size,
		str_output_size,
		NULL
	};

	pid_t check_pid = fork();

	if (check_pid < 0) {
		goto err_out;
	}

	if (check_pid == 0) {
		execv(problem_set->checker_path, argv);
		_exit(EXECUTE_UNKNOW);
	}

	int check_status;
	pid_t wait_check_pid = waitpid(check_pid, &check_status, 0);

	close(fd_shm); fd_shm = -1;
	close(fd_exp); fd_exp = -1;

	if (wait_check_pid < 0 || !WIFEXITED(check_status)) {
		return EXECUTE_UNKNOW;
	}

	execute_status_t check_code = WEXITSTATUS(check_status);
	if (is_execute_status(check_code)) {
		return check_code;
	} else {
		return EXECUTE_UNKNOW;
	}


err_out:
	if (fd_input_file > 0) {
		close(fd_input_file);
	}
	if (fd_shm > 0) {
		close(fd_shm);
	}
	if (fd_output_file > 0) {
		close(fd_output_file);
	}
	if (fd_exp > 0) {
		close(fd_exp);
	}

	return ret;
}

/* tmp */
int main()
{
	sandbox_path_t sandbox_path;
	snprintf(sandbox_path.base, sizeof(sandbox_path.base), "/tmp/pj/runtime");

	problem_set_t problem_set;
	snprintf(problem_set.input_path, sizeof(problem_set.input_path),
		"/tmp/pj_pro/0/input0.bin");
	snprintf(problem_set.output_path, sizeof(problem_set.output_path),
		"/tmp/pj_pro/0/output0.bin");
	snprintf(problem_set.checker_path, sizeof(problem_set.checker_path),
		"/tmp/pj_pro/0/checker.out");


	execute_status_t exe = execute(&sandbox_path, &problem_set);

	if (exe == EXECUTE_OK) {
		printf("SUCCES\n");
	} else {
		printf("FAIL\n");
	}

	return 0;
}
/* end tmp */
