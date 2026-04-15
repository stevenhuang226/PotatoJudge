#include "../include/utils/copy_fd.h"

#include <unistd.h>

copy_fd_t copy_fd(int src_fd, int dst_fd)
{
	int buffer_size = 4096;

	char buffer[buffer_size];

	for (;;) {
		ssize_t read_size = read(src_fd, buffer, sizeof(buffer));

		if (read_size == 0) {
			return COPY_FD_SUCCESS;
		}

		if (read_size < 0) {
			return COPY_FD_FAIL;
		}

		ssize_t offset = 0;

		while (offset < read_size) {
			ssize_t write_size = write(dst_fd, buffer + offset, read_size - offset);

			if (write_size < 0) {
				return COPY_FD_FAIL;
			}

			offset += write_size;
		}
	}
}
