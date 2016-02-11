#include "devfs.h"

#include <stdlib.h>
#include <string.h>

static fsnode_t devfs = {
    .name = "dev", .parent = NULL, .first_child = NULL, .next_sibling = NULL, .open = NULL,
};

static iodev_t *devfs_open_dev(devfsnode_t *node, int flags);

fsnode_t *devfs_get() {
	return &devfs;
}

void devfs_register_device(const char *devname, iodev_t *device) {
	devfsnode_t *node = malloc(sizeof(devfsnode_t));
	strcpy(node->node.name, devname);
	node->node.parent       = NULL;
	node->node.first_child  = NULL;
	node->node.next_sibling = NULL;
	node->node.open         = (fsopenfunc_t)devfs_open_dev;
	node->node.create       = NULL;
	node->device            = device;

	fs_link_child(&devfs, &node->node);
}

iodev_t *devfs_open_dev(devfsnode_t *node, int flags) {
	node->device->open(node->device, flags);
	return node->device;
}
