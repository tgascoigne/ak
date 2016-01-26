#include <kernel/syscall/syscall.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/module.h>
#include <kernel/panic.h>
#include <kernel/io/fdio.h>
#include <kernel/fs/node.h>

fd_t sys_open(const char *pathname, int flags, mode_t mode) {
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

	iodev_t *dev = fsn->open(fsn);
	if (dev == NULL) {
		PANIC("Couldn't open fs node");
		errno = ENOENT;
		return -1;
	}

	return fd_open(dev);
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

static bool fdio_init(void) {
	syscall_register(SYS_OPEN, (syscall_fn_t)sys_open, 3);
	syscall_register(SYS_CLOSE, (syscall_fn_t)sys_close, 1);
	syscall_register(SYS_READ, (syscall_fn_t)sys_read, 3);
	syscall_register(SYS_WRITE, (syscall_fn_t)sys_write, 3);
	return true;
}

module_init(fdio_init);
