#include "init.h"

#include <stdio.h>

#include <arch/x86/boot/multiboot.h>
#include <arch/x86/boot/multibootimpl.h>
#include <kernel/panic.h>

void arch_init(multiboot_info_t* mb_info) {
	multiboot_mmap(mb_info);
}
