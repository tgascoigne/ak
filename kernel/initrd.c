#include "initrd.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mem/map.h>
#include <mem/mmu.h>

paddr_t InitrdStart, InitrdEnd;
cpiohdr_t *Initrd;

void initrd_mount(fsnode_t *mountpoint) {
	fsnode_t *ird = cpiofs_mount("initrd", Initrd);
	if (ird == NULL) {
		printf("Unable to mount initrd\n");
		return;
	}
	fs_link_child(mountpoint, ird);
}
