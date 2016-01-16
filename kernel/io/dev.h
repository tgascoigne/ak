#pragma once

#include <unistd.h>

typedef int (*ioopenfunc_t)(int flags);
typedef ssize_t (*ioreadfunc_t)(void *buf, size_t count);
typedef ssize_t (*iowritefunc_t)(const void *buf, size_t count);
typedef int (*ioclosefunc_t)();

typedef struct {
	ioopenfunc_t open;
	ioreadfunc_t read;
	iowritefunc_t write;
	ioclosefunc_t close;
} iodev_t;
