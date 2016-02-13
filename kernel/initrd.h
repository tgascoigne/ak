#pragma once

#include <mem/types.h>
#include <kernel/fs/cpiofs.h>

extern paddr_t InitrdStart, InitrdEnd;
extern cpiohdr_t *Initrd;

void initrd_mount(fsnode_t *mountpoint);
