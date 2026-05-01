#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define MIN(x, y) (((x) < (y)) ? (x) : (y))
int main(int argc, char **argv)
{
	int shm_fd = -1;
	int exp_fd = -1;
	off_t shm_size = -1;
	off_t exp_size = -1;

	if (argc < 5) {
		goto err_out;
	}

	shm_fd = atoi(argv[1]);
	exp_fd = atoi(argv[2]);
	shm_size = atoi(argv[3]);
	exp_size = atoi(argv[4]);

	char *shm_ptr = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		goto err_out;
	}

	char *exp_ptr = mmap(NULL, exp_size, PROT_READ, MAP_SHARED, exp_fd, 0);
	if (exp_ptr == MAP_FAILED) {
		goto err_out;
	}

	int cmp_length = MIN(shm_size, exp_size);
	int cmp = strncmp(shm_ptr, exp_ptr, cmp_length);

	munmap(shm_ptr, shm_size);
	munmap(exp_ptr, exp_size);

	close(shm_fd); shm_fd = -1;
	close(exp_fd); exp_fd = -1;

	if (cmp == 0) {
		return 0;
	} else {
		return 1;
	}

err_out:
	if (shm_fd >= 0) {
		close(shm_fd); shm_fd = -1;
	}
	if (exp_fd >= 0) {
		close(exp_fd); exp_fd = -1;
	}
	return -1;
}
