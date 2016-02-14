#include "fdio.h"

#include <asm/ioctls.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include <kernel/syscall/syscall.h>
#include <kernel/panic.h>
#include <kernel/io/fdio.h>
#include <kernel/io/stat.h>
#include <kernel/fs/node.h>
#include <kernel/proc/task.h>

int sys_open(const char *pathname, int flags, mode_t mode) {
	fsnode_t *fsn = fs_locate(FSRootNode, pathname);
	if (fsn == NULL) {
		if ((flags & O_CREAT) != 0) {
#pragma message("create file")
			errno = EACCES;
			return -1;
		} else {
			errno = ENOENT;
			return -1;
		}
	}

	iodev_t *dev = fsn->open(fsn, 0);
	if (dev == NULL) {
		PANIC("Couldn't open fs node\n");
		errno = ENOENT;
		return -1;
	}

	return (int)fd_open(dev);
}

ssize_t sys_write(fd_t fd, const void *buf, size_t nbyte) {
	fdescr_t *descr = fd_lookup(fd);
	if (descr == NULL) {
		errno = EBADF;
		return -1;
	}

	iodev_t *dev = descr->dev;
	return dev->write(dev, buf, nbyte);
}

ssize_t sys_read(fd_t fd, void *buf, size_t nbyte) {
	fdescr_t *descr = fd_lookup(fd);
	if (descr == NULL) {
		errno = EBADF;
		return -1;
	}

	iodev_t *dev = descr->dev;
	return dev->read(dev, buf, nbyte);
}

int sys_close(fd_t fd) {
	fdescr_t *descr = fd_lookup(fd);
	if (descr == NULL) {
		errno = EBADF;
		return -1;
	}

	iodev_t *dev = descr->dev;
	int dev_ret  = dev->close(dev);
	int fd_ret   = fd_close(fd);

	if (dev_ret == -1 || fd_ret == -1) {
		return -1;
	}

	return 0;
}

int sys_ioctl(fd_t fd, unsigned int cmd, unsigned long arg) {
	int *iptr = (int *)arg;
	switch (cmd) {
	case TIOCGPGRP:
		*iptr = CurrentTask->pgid;
		return 0;
		break;
	}
	return 0;
}

int sys_fcntl(fd_t fd, unsigned int cmd, unsigned long arg) {
	switch (cmd) {
	case F_DUPFD:
		return fd_dupfd(fd, (fd_t)arg);
	}
	return 0;
}

int sys_newstat(const char *path, struct kernel_stat *stat) {
	fsnode_t *fsn = fs_locate(FSRootNode, path);
	if (fsn == NULL) {
		errno = ENOENT;
		return -1;
	}
	memset(stat, 0, sizeof(struct kernel_stat));

	return fsn->stat(fsn, stat);
}

int sys_getcwd(char *buf, size_t size) {
	strncpy(buf, CurrentTask->cwd, size);
	return (int)strlen(buf);
}

void fdio_init(void) {
	syscall_register(SYS_OPEN, (syscall_fn_t)sys_open, 3);
	syscall_register(SYS_CLOSE, (syscall_fn_t)sys_close, 1);
	syscall_register(SYS_IOCTL, (syscall_fn_t)sys_ioctl, 3);
	syscall_register(SYS_FCNTL, (syscall_fn_t)sys_fcntl, 3);
	syscall_register(SYS_GETCWD, (syscall_fn_t)sys_getcwd, 2);
	syscall_register(SYS_READ, (syscall_fn_t)sys_read, 3);
	syscall_register(SYS_WRITE, (syscall_fn_t)sys_write, 3);
	syscall_register(SYS_NEWSTAT, (syscall_fn_t)sys_newstat, 2);
	fdio_tbl_init();
}
