#ifndef EXECUTE_RESOURCE_H
#define EXECUTE_RESOURCE_H

#include <stdint.h>

typedef struct {
	uint64_t time_us;
	uint64_t mem_kb;
} execute_resource_t;

#endif
