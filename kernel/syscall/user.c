#include "user.h"

#include <kernel/syscall/syscall.h>

int sys_fake_getid(void) {
	return 1000;
}

void user_syscall_init(void) {
	syscall_register(SYS_GETUID, (syscall_fn_t)sys_fake_getid, 0);
	syscall_register(SYS_GETGID, (syscall_fn_t)sys_fake_getid, 0);
	syscall_register(SYS_GETEUID, (syscall_fn_t)sys_fake_getid, 0);
	syscall_register(SYS_GETEGID, (syscall_fn_t)sys_fake_getid, 0);
}
