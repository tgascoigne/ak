#include "initrd.h"

cpiohdr_t *Initrd;

void initrd_mount(fsnode_t *mountpoint) {
	fsnode_t *ird = cpiofs_mount("initrd", Initrd);
	fs_link_child(mountpoint, ird);
}
