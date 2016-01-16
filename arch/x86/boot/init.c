#include "init.h"

#include <stdio.h>

#include <arch/x86/boot/multiboot.h>
#include <arch/x86/boot/multibootimpl.h>
#include <arch/x86/intr/idt.h>
#include <kernel/panic.h>
#include <kernel/task.h>

extern void *_kern_end;

void arch_init(multiboot_info_t *mb_info) {
	KernelTask.brk = (vaddr_t)&_kern_end;

	idt_init();
	multiboot_mmap(mb_info);
}
