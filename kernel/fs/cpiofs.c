#include "cpiofs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/io/stat.h>
#include <mem/types.h>

static cpiohdr_t *cpiofs_unpack_file(fsnode_t *parent, cpiohdr_t *hdr);
static iodev_t *cpiofs_open_file(cpionode_t *node);
static ssize_t cpiofs_read_file(cpiofiledev_t *file, void *buf, size_t nbyte);
static off_t cpiofs_seek_file(cpiofiledev_t *file, off_t offset, int whence);
static void cpiofs_close_file(cpiofiledev_t *file);
static int cpiofs_stat_file(cpionode_t *node, struct kernel_stat *stat);

static fsnode_t cpiofs_dir_tmpl = {
    .parent = NULL, .first_child = NULL, .next_sibling = NULL, .open = NULL, .create = NULL,
};

static fsnode_t cpiofs_file_tmpl = {
    .parent		      = NULL,
    .first_child		 = NULL,
    .next_sibling		= NULL,
    .open			= (fsopenfunc_t)cpiofs_open_file,
    .create		      = NULL,
    .stat			= (fsstatfunc_t)cpiofs_stat_file,
};

static cpiohdr_t CpioEOF;

static iodev_t cpiofiledev_tmpl = {
    .open			      = NULL,
    .close			     = (ioclosefunc_t)cpiofs_close_file,
    .write			     = NULL, /* read only */
    .read			      = (ioreadfunc_t)cpiofs_read_file,
    .seek			      = (ioseekfunc_t)cpiofs_seek_file,
};

fsnode_t *cpiofs_mount(const char *name, cpiohdr_t *cpio_hdr) {
	cpionode_t *cpiofs = malloc(sizeof(cpionode_t));
	memcpy(cpiofs, &cpiofs_dir_tmpl, sizeof(fsnode_t));
	strcpy(cpiofs->node.name, name);

	while (cpio_hdr != &CpioEOF) {
		cpio_hdr = cpiofs_unpack_file((fsnode_t *)cpiofs, cpio_hdr);
		if (cpio_hdr == NULL) {
			return NULL;
		}
	}

	return (fsnode_t *)cpiofs;
}

cpiohdr_t *cpiofs_unpack_file(fsnode_t *parent, cpiohdr_t *hdr) {
	if (hdr->magic != CPIO_MAGIC) {
		printf("error parsing cpiofs: invalid magic %o\n", hdr->magic);
		return NULL;
	}

	const char *name = (const char *)((vaddr_t)hdr + sizeof(cpiohdr_t));
	if (strcmp(name, "TRAILER!!!") == 0) {
		/* last entry in the archive is named TRAILER!!! */
		return &CpioEOF;
	}

	const char *data = name + hdr->namesize;
	if ((hdr->namesize % 2) != 0) {
		/* namesize is rounded up to the nearest 2 byte boundary */
		data++;
	}

	// For some reason the low and high u16s are swapped in u32 values
	hdr->filesize = ((hdr->filesize >> 16) & 0xFFFF) + ((hdr->filesize << 16) & 0xFFFF0000);
	hdr->mtime    = ((hdr->mtime >> 16) & 0xFFFF) + ((hdr->mtime << 16) & 0xFFFF0000);

	cpiofiledev_t *file = (cpiofiledev_t *)malloc(sizeof(cpiofiledev_t));
	memcpy(&file->iodev, &cpiofiledev_tmpl, sizeof(iodev_t));
	file->pos    = 0;
	file->length = (ssize_t)hdr->filesize;
	file->data   = data;

	cpionode_t *node = (cpionode_t *)malloc(sizeof(cpionode_t));
	memcpy(&node->node, &cpiofs_file_tmpl, sizeof(fsnode_t));
	node->device = (iodev_t *)file;
	strcpy(node->node.name, name);

	fs_link_child(parent, (fsnode_t *)&node->node);

	if ((hdr->filesize % 2) != 0) {
		/* file is padded to an even length */
		hdr->filesize++;
	}

	cpiohdr_t *next_hdr = (cpiohdr_t *)(data + hdr->filesize);
	return next_hdr;
}

iodev_t *cpiofs_open_file(cpionode_t *node) {
	return node->device;
}

ssize_t cpiofs_read_file(cpiofiledev_t *file, void *buf, size_t nbyte) {
	if ((size_t)file->pos + nbyte > (size_t)file->length) {
		nbyte = (size_t)file->length - (size_t)file->pos;
	}
	memcpy(buf, file->data + file->pos, nbyte);
	file->pos += nbyte;
	return (ssize_t)nbyte;
}

int cpiofs_stat_file(cpionode_t *node, struct kernel_stat *stat) {
	cpiofiledev_t *dev = (cpiofiledev_t *)cpiofs_open_file(node);
	if (dev == NULL) {
		return -1;
	}

	stat->st_size = (unsigned long)dev->length;
	return 0;
}

off_t cpiofs_seek_file(cpiofiledev_t *file, off_t offset, int whence) {
	if (whence == SEEK_SET) {
		file->pos = offset;
	} else if (whence == SEEK_CUR) {
		file->pos = file->pos + offset;
	} else if (whence == SEEK_END) {
		file->pos = file->length - offset;
	}
	return file->pos;
}

void cpiofs_close_file(cpiofiledev_t *file) {
}
