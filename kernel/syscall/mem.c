#include "mem.h"

#include <sys/mman.h>

#include <arch/i386/mem/brk.h>
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
#pragma message("mmap allocations aren't done properly")
		void *mem = (void *)CurrentTask->mmapbrk;
		pg_alloc_range((vaddr_t)mem, (vaddr_t)mem + length + PAGE_SIZE, true);
		CurrentTask->mmapbrk = (vaddr_t)mem + length + PAGE_SIZE;
		return mem;
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
}
