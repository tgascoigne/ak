#pragma once

#include <stdint.h>

#include <kernel/fs/node.h>

typedef struct {
	fsnode_t node;
	iodev_t *device;
} tmpnode_t;

typedef struct {
	iodev_t iodev;
	off_t pos;
	ssize_t length;
	char *data;
} tmpfiledev_t;

fsnode_t *tmpfs_create(const char *name);
fsnode_t *tmpfs_create_file(struct fsnode *parent, const char *name);
