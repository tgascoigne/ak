#include "uname.h"

#include <sys/utsname.h>

#include <kernel/syscall/syscall.h>

int sys_uname(struct utsname *buf) {
	sprintf(buf->sysname, "ak");
	sprintf(buf->nodename, "localhost");
	sprintf(buf->release, "0.0");
	sprintf(buf->version, "1");
	sprintf(buf->machine, "qemu");
	sprintf(buf->__domainname, "localdomain");
	return 0;
}

void uname_syscall_init(void) {
	syscall_register(SYS_UNAME, (syscall_fn_t)sys_uname, 1);
}
