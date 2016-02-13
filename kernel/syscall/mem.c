#include "mem.h"

#include <sys/mman.h>

#include <arch/x86/mem/brk.h>
#include <arch/x86/mem/early_malloc.h>
#include <kernel/panic.h>
#include <kernel/syscall/syscall.h>

struct mmap_args {
	void *addr;
	size_t length;
	int prot;
	int flags;
	int fd;
	off_t offset;
};

void *sys_brk(vaddr_t brk) {
	return (void *)task_brk(CurrentTask, brk);
}

void *sys_mmap_pgoff(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	if ((flags & MAP_ANONYMOUS) != 0) {
		void *new_brk = sys_brk((vaddr_t)sys_brk(0) + (vaddr_t)length);
		return (void *)((size_t)new_brk - length);
	}
	PANIC("sys_mmap: not implemented\n");
	return (void *)-1;
}

void *sys_mmap(struct mmap_args *args) {
	return sys_mmap_pgoff(args->addr, args->length, args->prot, args->flags, args->fd, args->offset);
}

void mem_syscall_init(void) {
	syscall_register(SYS_BRK, (syscall_fn_t)sys_brk, 1);
	syscall_register(SYS_MMAP, (syscall_fn_t)sys_mmap, 1);
	syscall_register(SYS_MMAP_PGOFF, (syscall_fn_t)sys_mmap_pgoff, 6);
	early_malloc_disable();
}
