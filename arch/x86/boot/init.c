#include "init.h"

#include <stdio.h>

#include <arch/x86/boot/multiboot.h>
#include <arch/x86/boot/multibootimpl.h>
#include <arch/x86/mem/map.h>
#include <arch/x86/mem/framealloc.h>
#include <arch/x86/intr/idt.h>
#include <kernel/panic.h>
#include <kernel/proc/task.h>

extern void *_kern_end;

void arch_init(multiboot_info_t *mb_info) {
	KernelTask.brk = (vaddr_t)&_kern_end;

	idt_init();
	multiboot_mmap(mb_info);
	/* the first 4m is used for various devices and the kernel */
	frame_set_range(0, 0x400000);
	pg_init();
}
