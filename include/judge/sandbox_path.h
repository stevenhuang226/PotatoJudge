#ifndef SANDBOX_PATH_H
#define SANDBOX_PATH_H

#include <stdio.h>

typedef struct {
	char base[MAX_PATH_LENGTH];
	char usr[MAX_PATH_LENGTH];
	char lib[MAX_PATH_LENGTH];
	char lib64[MAX_PATH_LENGTH];
	char tmp[MAX_PATH_LENGTH];
	char dev[MAX_PATH_LENGTH];
	char dev_null[MAX_PATH_LENGTH];
} sandbox_path_t;

#endif
