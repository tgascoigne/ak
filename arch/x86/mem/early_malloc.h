#pragma once

#include <stdlib.h>

void *early_malloc(size_t size);
void early_free(void *ptr);
void early_malloc_disable(void);
