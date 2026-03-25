#include <stdlib.h>
#include <sys/mman.h>

void *create_shared(ssize_t size)
{
	void *shared = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}
