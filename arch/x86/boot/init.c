#include "init.h"

#include <stdio.h>

#include <arch/x86/boot/multiboot.h>

bool multiboot_validate(unsigned long magic, multiboot_info_t* mb_info) {
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("bad multiboot magic\n");
		return false;
	}

	return true;
}
