#include "init.h"

#include <stdio.h>

#include <arch/i386/boot/multibootimpl.h>
#include <arch/i386/mem/framealloc.h>
#include <arch/i386/mem/gdt.h>
#include <arch/i386/intr/idt.h>
#include <arch/i386/intr/pit.h>
#include <kernel/proc/task.h>

void arch_init(vaddr_t mb_info) {
	gdt_init();
	idt_init();
	pg_init();
	pit_init();
	multiboot_mmap(mb_info);
	/* multiboot_mmap will have identity mapped up to brk without claiming the frames */
	frame_set_range(0, 0x1000 * 0x1000);
	pg_unmap_low_kernel();
	/* alloc the first page for TLS */
	pg_alloc(0x0);
}
