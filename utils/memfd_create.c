#include "../include/utils/potato_try.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/syscall.h>
//#include <linux/fcntl.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int memfd_create_wrap(const char *name, const off_t limit)
{
	int ret_err = -1;
	int fd = -1;

	if (limit <= 0) {
		goto err_out;
	}

	TRY_GIVE(syscall(SYS_memfd_create, name, MFD_ALLOW_SEALING), fd);
	TRY_NOEQU(ftruncate(fd, limit), 0);
	TRY(fcntl(fd, F_ADD_SEALS, F_SEAL_GROW | F_SEAL_SHRINK | F_SEAL_SEAL));

	return fd;
err_out:
	if (fd >= 0) {
		close(fd); fd = -1;
	}
	return ret_err;
}
