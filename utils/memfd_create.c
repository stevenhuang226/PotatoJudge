#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>

int memfd_create_wrap(const char *name, const off_t limit)
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
