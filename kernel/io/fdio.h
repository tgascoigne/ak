#pragma once

#include <stdbool.h>

#include <kernel/io/dev.h>

typedef int fd_t;

#define FD_MAX (fd_t)0xFFFF

typedef struct { iodev_t *dev; } fdescr_t;

fd_t fd_open(iodev_t *dev);
fdescr_t *fd_lookup(fd_t fd);
int fd_close(fd_t fd);
bool fdio_tbl_init(void);