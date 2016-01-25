#include "init.h"

#include <stdio.h>
#include <string.h>

#include <arch/x86/boot/multiboot.h>
#include <arch/x86/boot/multibootimpl.h>
#include <arch/x86/mem/brk.h>
#include <arch/x86/mem/map.h>
#include <arch/x86/mem/framealloc.h>
#include <arch/x86/intr/idt.h>
#include <arch/x86/intr/intr.h>
#include <kernel/panic.h>
#include <kernel/kmain.h>
#include <kernel/proc/task.h>

void arch_init(multiboot_info_t *mb_info) {
	idt_init();
	multiboot_mmap(mb_info);
	/* the first 4m is used for various devices and the kernel */
	frame_set_range(0, 0x400000);
	pg_init();

	intr_enable();

	task_brk(CurrentTask, KZERO + 0x400000 + (0x4000 * 4));
	char *x = (char *)KZERO + 0x400000;
	memset(x, 0, 0x4000 * 4);
}
