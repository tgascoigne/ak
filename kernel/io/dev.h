#pragma once

#include <unistd.h>

struct iodev;

typedef int (*ioopenfunc_t)(struct iodev *dev, int flags);
typedef ssize_t (*ioreadfunc_t)(struct iodev *dev, void *buf, size_t count);
typedef ssize_t (*iowritefunc_t)(struct iodev *dev, const void *buf, size_t count);
typedef int (*ioclosefunc_t)(struct iodev *dev);

typedef struct iodev {
	ioopenfunc_t open;
	ioreadfunc_t read;
	iowritefunc_t write;
	ioclosefunc_t close;
} iodev_t;
