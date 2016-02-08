#pragma once

#include <kernel/fs/cpiofs.h>

extern cpiohdr_t *Initrd;

void initrd_mount(fsnode_t *mountpoint);
