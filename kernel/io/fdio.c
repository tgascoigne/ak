#include "fdio.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/proc/task.h>

static fd_t fd_alloc(void) {
	while (CurrentTask->fdtbl[CurrentTask->fdnext] != NULL) {
		CurrentTask->fdnext++;
	}

	if (CurrentTask->fdnext >= CurrentTask->fdcap) {
		fd_t oldcap = CurrentTask->fdcap;
		CurrentTask->fdcap *= 2;
		CurrentTask->fdtbl = (fdescr_t **)realloc(CurrentTask->fdtbl, CurrentTask->fdcap * sizeof(fdescr_t *));
		memset(&CurrentTask->fdtbl[oldcap], 0, (CurrentTask->fdcap - oldcap) * sizeof(fdescr_t *));
	}

	return CurrentTask->fdnext;
}

fd_t fd_open(iodev_t *dev) {
	fd_t fd                = fd_alloc();
	fdescr_t *descr        = (fdescr_t *)malloc(sizeof(fdescr_t));
	descr->dev             = dev;
	descr->refs            = 1;
	CurrentTask->fdtbl[fd] = descr;
	return fd;
}

fdescr_t *fd_lookup(fd_t fd) {
	return CurrentTask->fdtbl[fd];
}

int fd_close(fd_t fd) {
	if (CurrentTask->fdtbl[fd] == NULL) {
		errno = EBADF;
		return -1;
	} else {
		CurrentTask->fdtbl[fd]->refs--;
		if (CurrentTask->fdtbl[fd]->refs == 0) {
			free(CurrentTask->fdtbl[fd]);
		}
		CurrentTask->fdtbl[fd] = 0;
	}

	return 0;
}

bool fdio_tbl_init(void) {
	CurrentTask->fdnext = 0;
	CurrentTask->fdcap  = 16;
	CurrentTask->fdtbl = (fdescr_t **)malloc(CurrentTask->fdcap * sizeof(fdescr_t *));
	memset(CurrentTask->fdtbl, 0, FD_MAX * sizeof(fdescr_t *));
	return true;
}
