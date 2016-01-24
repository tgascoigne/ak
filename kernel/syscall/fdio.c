#include <kernel/syscall/syscall.h>

#include <unistd.h>

#include <kernel/module.h>
#include <kernel/io/tty.h>

ssize_t sys_write(int fildes, const void *buf, size_t nbyte) {
	return KConsole->write(KConsole, buf, nbyte);
}

static bool fdio_init(void) {
	syscall_register(SYS_WRITE, (syscall_fn_t)sys_write, 3);
	return true;
}

module_init(fdio_init);
