#include "../include/utils/copy_file.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

copy_file_t copy_file(const char *src_path, const char *dst_path)
{
	int dst_permissions = 0644;
	int buffer_size = 4096;

	int src_fd = open(src_path, O_RDONLY);
	if (src_fd < 0) {
		return COPY_FILE_NOT_FOUND;
	}

	int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, dst_permissions);
	if (dst_fd < 0) {
		close(src_fd);
		return COPY_FILE_FAILED;
	}

	char buffer[buffer_size];
	ssize_t read_size;

	for (;;) {
		read_size = read(src_fd, buffer, sizeof(buffer));

		if (read_size == 0) {
			break;
		}

		if (read_size < 0) {
			close(src_fd);
			close(dst_fd);
			return COPY_FILE_FAILED;
		}

		ssize_t written = write(dst_fd, buffer, read_size);
		if (written != read_size) {
			close(src_fd);
			close(dst_fd);
			return COPY_FILE_FAILED;
		}
	}

	close(src_fd);
	close(dst_fd);
	return COPY_FILE_SUCCESS;
}
