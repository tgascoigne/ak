#include "init.h"

#include <stdio.h>

#include <arch/x86/boot/multibootimpl.h>
#include <arch/x86/mem/framealloc.h>
#include <arch/x86/mem/gdt.h>
#include <arch/x86/intr/idt.h>
#include <arch/x86/intr/pit.h>
#include <kernel/proc/task.h>

void arch_init(vaddr_t mb_info) {
	gdt_init();
	idt_init();
	multiboot_mmap(mb_info);
	/* the first 4m is used for various devices and the kernel */
	frame_set_range(0, 0x400000);
	pg_init();
	pit_init();
}
