#include "multibootimpl.h"

#include <arch/hwinfo.h>
#include <arch/x86/mem/framealloc.h>
#include <kernel/panic.h>

bool multiboot_validate(uint32_t magic, multiboot_info_t *mb_info) {
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("bad multiboot magic %i\n", magic);
	}

	return true;
}

void multiboot_mmap(multiboot_info_t *mb_info) {
	if ((mb_info->flags & MULTIBOOT_INFO_MEMORY) == 0) {
		PANIC("MULTIBOOT_INFO_MEMORY missing\n");
	}

	if ((mb_info->flags & MULTIBOOT_INFO_MEM_MAP) == 0) {
		PANIC("MULTIBOOT_INFO_MEM_MAP missing\n");
	}

	/* mem_upper is in KB */
	HardwareInfo.mem_max = mb_info->mem_upper * 1024;
	frame_alloc_init(HardwareInfo.mem_max);

	multiboot_memory_map_t *r = (multiboot_memory_map_t *)mb_info->mmap_addr;
	while ((multiboot_uint32_t)r < mb_info->mmap_addr + mb_info->mmap_length) {
		if (r->type != 1) {
			/* type == 1 means usable ram; everything else is reserved */
			frame_set_range((paddr_t)r->addr, (paddr_t)r->addr + (paddr_t)r->len);
		}
		r = (multiboot_memory_map_t *)((paddr_t)r + r->size + sizeof(multiboot_uint32_t));
	}
}
