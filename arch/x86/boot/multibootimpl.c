#include "multibootimpl.h"

#include <stdlib.h>
#include <string.h>

#include <arch/hwinfo.h>
#include <arch/x86/mem/framealloc.h>
#include <arch/x86/mem/map.h>
#include <kernel/panic.h>
#include <kernel/initrd.h>

bool multiboot_validate(uint32_t magic, vaddr_t mb_info) {
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		PANIC("bad multiboot magic %i\n", magic);
	}

	return true;
}

void multiboot_mmap(vaddr_t mb_info) {
	// the first 8 bytes are size + reserved
	struct multiboot_tag *tag = (struct multiboot_tag *)(mb_info + 8);

	while (tag->type != MULTIBOOT_TAG_TYPE_END) {
		if (tag->type != MULTIBOOT_TAG_TYPE_BASIC_MEMINFO) {
			tag = (struct multiboot_tag *)((vaddr_t)tag + ((tag->size + 7) & (uint32_t)~7));
			continue;
		}

		struct multiboot_tag_basic_meminfo *meminfo = (struct multiboot_tag_basic_meminfo *)tag;

		/* mem_upper is the amount of upper memory in kb */
		/* upper memory begins at 1M (hence + 1024) */
		HardwareInfo.mem_max = (meminfo->mem_upper + 1024) * 1024;
		frame_alloc_init(HardwareInfo.mem_max);

		break;
	}

	tag = (struct multiboot_tag *)(mb_info + 8);

	paddr_t initrd_start;
	paddr_t initrd_len;

	while (tag->type != MULTIBOOT_TAG_TYPE_END) {
		if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
			struct multiboot_tag_mmap *mmap    = (struct multiboot_tag_mmap *)tag;
			struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)&mmap->entries;
			for (int i = 0; i < (int)tag->size; i += mmap->entry_size) {
				if (entry->type == MULTIBOOT_MEMORY_RESERVED) {
					frame_set_range((paddr_t)entry->addr,
							(paddr_t)entry->addr + (paddr_t)entry->len);
				}

				entry = (struct multiboot_mmap_entry *)((vaddr_t)entry + (vaddr_t)mmap->entry_size);
			}
		}

		if (tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
			struct multiboot_tag_module *mod = (struct multiboot_tag_module *)tag;

			initrd_start = mod->mod_start;
			initrd_len   = mod->mod_end - mod->mod_start;
		}

		tag = (struct multiboot_tag *)((vaddr_t)tag + ((tag->size + 7) & (uint32_t)~7));
	}

	Initrd = (cpiohdr_t *)malloc(initrd_len);
	memcpy(Initrd, (void *)initrd_start, initrd_len);
}
