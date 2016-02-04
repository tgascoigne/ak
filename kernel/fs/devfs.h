#pragma once

#include <kernel/fs/node.h>

typedef struct {
	fsnode_t node;
	iodev_t *device;
} devfsnode_t;

fsnode_t *devfs_get();
void devfs_register_device(const char *devname, iodev_t *device);
