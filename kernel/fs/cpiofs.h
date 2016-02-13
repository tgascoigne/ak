#pragma once

#include <stdint.h>

#include <kernel/fs/node.h>

#define CPIO_MAGIC 070707

typedef struct {
	uint16_t magic;
	uint16_t dev;
	uint16_t ino;
	uint16_t mode;
	uint16_t uid;
	uint16_t gid;
	uint16_t nlink;
	uint16_t rdev;
	uint16_t mtime[2];
	uint16_t namesize;
	uint16_t filesize[2];
} __attribute__((packed)) cpiohdr_t;

typedef struct {
	fsnode_t node;
	iodev_t *device;
} cpionode_t;

typedef struct {
	iodev_t iodev;
	off_t pos;
	ssize_t length;
	cpiohdr_t *hdr;
	const char *data;
} cpiofiledev_t;

fsnode_t *cpiofs_mount(const char *name, cpiohdr_t *cpio_hdr);
