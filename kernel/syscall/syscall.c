#include "syscall.h"

int syscall_argc[NUM_SYSCALLS]           = {0};
syscall_fn_t syscall_funcs[NUM_SYSCALLS] = {0};

void syscall_register(int syscall, syscall_fn_t fn, int argc) {
	syscall_funcs[syscall] = fn;
	syscall_argc[syscall]  = argc;
}
