#include "tmpfs.h"

#include <stdlib.h>
#include <string.h>

static iodev_t *tmpfs_open_file(tmpnode_t *node);
static ssize_t tmpfs_write_file(tmpfiledev_t *file, const void *buf, size_t nbyte);
static ssize_t tmpfs_read_file(tmpfiledev_t *file, void *buf, size_t nbyte);
static off_t tmpfs_seek_file(tmpfiledev_t *file, off_t offset, int whence);
static void tmpfs_close_file(tmpfiledev_t *file);

static fsnode_t tmpfs_dir_tmpl = {
    .parent = NULL, .first_child = NULL, .next_sibling = NULL, .open = NULL, .create = tmpfs_create_file,
};

static fsnode_t tmpfs_file_tmpl = {
    .parent = NULL, .first_child = NULL, .next_sibling = NULL, .open = (fsopenfunc_t)tmpfs_open_file, .create = NULL,
};

static iodev_t tmpfiledev_tmpl = {
    .open                      = NULL,
    .close                     = (ioclosefunc_t)tmpfs_close_file,
    .write                     = (iowritefunc_t)tmpfs_write_file,
    .read                      = (ioreadfunc_t)tmpfs_read_file,
    .seek                      = (ioseekfunc_t)tmpfs_seek_file,
};

fsnode_t *tmpfs_create(const char *name) {
	fsnode_t *tmpfs = malloc(sizeof(fsnode_t));
	memcpy(tmpfs, &tmpfs_dir_tmpl, sizeof(fsnode_t));
	strcpy(tmpfs->name, name);
	return tmpfs;
}

fsnode_t *tmpfs_create_file(struct fsnode *parent, const char *name) {
	tmpfiledev_t *file = (tmpfiledev_t *)malloc(sizeof(tmpfiledev_t));
	memcpy(&file->iodev, &tmpfiledev_tmpl, sizeof(iodev_t));
	file->pos    = 0;
	file->length = 0;
	file->data   = (char *)malloc(0);

	tmpnode_t *node = (tmpnode_t *)malloc(sizeof(tmpnode_t));
	memcpy(&node->node, &tmpfs_file_tmpl, sizeof(fsnode_t));
	node->device = (iodev_t *)file;
	strcpy(node->node.name, name);

	fs_link_child(parent, (fsnode_t *)&node->node);

	return (fsnode_t *)&node->node;
}

iodev_t *tmpfs_open_file(tmpnode_t *node) {
	return node->device;
}

ssize_t tmpfs_write_file(tmpfiledev_t *file, const void *buf, size_t nbyte) {
	if ((size_t)file->pos + nbyte > (size_t)file->length) {
		file->data   = realloc(file->data, (size_t)file->pos + (size_t)nbyte);
		file->length = (ssize_t)file->pos + (ssize_t)nbyte;
	}
	memcpy(file->data + (size_t)file->pos, buf, nbyte);
	file->pos += (off_t)nbyte;
	return (ssize_t)nbyte;
}

ssize_t tmpfs_read_file(tmpfiledev_t *file, void *buf, size_t nbyte) {
	if ((size_t)file->pos + nbyte > (size_t)file->length) {
		nbyte = (size_t)file->length - (size_t)file->pos;
	}
	memcpy(buf, file->data + file->pos, nbyte);
	file->pos += (off_t)nbyte;
	return (ssize_t)nbyte;
}

off_t tmpfs_seek_file(tmpfiledev_t *file, off_t offset, int whence) {
	if (whence == SEEK_SET) {
		file->pos = offset;
	} else if (whence == SEEK_CUR) {
		file->pos = file->pos + offset;
	} else if (whence == SEEK_END) {
		file->pos = file->length - offset;
	}
	return file->pos;
}

void tmpfs_close_file(tmpfiledev_t *file) {
	free(file->data);
}
