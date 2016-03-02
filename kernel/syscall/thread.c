#include "thread.h"

#include <proc/tls.h>
#include <kernel/syscall/syscall.h>

int sys_get_thread_area(struct user_desc *u_info) {
	return arch_get_thread_area(u_info);
}

int sys_set_thread_area(struct user_desc *u_info) {
	return arch_set_thread_area(u_info);
}

void thread_init(void) {
	syscall_register(SYS_SET_THREAD_AREA, (syscall_fn_t)sys_set_thread_area, 1);
	syscall_register(SYS_GET_THREAD_AREA, (syscall_fn_t)sys_get_thread_area, 1);
}
