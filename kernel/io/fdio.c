#include "fdio.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/proc/task.h>

static fd_t next_fd = 0;
static fdescr_t **fd_table;
static fdescr_t SpecialFDescr;

static fd_t fd_alloc(void) {
	while (fd_table[next_fd] != NULL) {
		next_fd++;
	}
	return next_fd;
}

fd_t fd_open(iodev_t *dev) {
	fd_t fd		= fd_alloc();
	fdescr_t *descr = (fdescr_t *)malloc(sizeof(fdescr_t));
	descr->dev      = dev;
	fd_table[fd]    = descr;
	return fd;
}

fdescr_t *fd_lookup(fd_t fd) {
	switch (fd) {
	case 0:
	case 1:
	case 2:
		return CurrentTask->console;

	default:
		break;
	}

	return fd_table[fd];
}

int fd_close(fd_t fd) {
	if (fd_table[fd] == NULL) {
		errno = EBADF;
		return -1;
	} else if (fd_table[fd] == &SpecialFDescr) {
	} else {
		free(fd_table[fd]);
		fd_table[fd] = 0;
	}

	return 0;
}

bool fdio_tbl_init(void) {
	fd_table = (fdescr_t **)malloc(FD_MAX * sizeof(fdescr_t *));
	memset(fd_table, 0, FD_MAX * sizeof(fdescr_t *));

	/* reserve fds 0-2 for console io */
	fd_table[0] = &SpecialFDescr;
	fd_table[1] = &SpecialFDescr;
	fd_table[2] = &SpecialFDescr;
	return true;
}
