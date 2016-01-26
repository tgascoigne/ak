#include <sys/mman.h>

#include <arch/x86/mem/brk.h>
#include <arch/x86/mem/early_malloc.h>
#include <kernel/module.h>
#include <kernel/panic.h>
#include <kernel/syscall/syscall.h>

void *sys_brk(vaddr_t brk) {
	return (void *)task_brk(CurrentTask, brk);
}

void *sys_mmap(void *addr, size_t length, int *prot, int *flags, int *fd, off_t *offset) {
	if ((*flags & MAP_ANONYMOUS) != 0) {
		return sys_brk((vaddr_t)sys_brk(0) + (vaddr_t)length);
	}
	PANIC("sys_mmap: not implemented\n");
	return (void *)-1;
}

static bool mem_syscall_init(void) {
	syscall_register(SYS_BRK, (syscall_fn_t)sys_brk, 1);
	syscall_register(SYS_MMAP, (syscall_fn_t)sys_mmap, 6);
	early_malloc_disable();
	return true;
}

module_init_prio(mem_syscall_init, MODINIT_MEM);
