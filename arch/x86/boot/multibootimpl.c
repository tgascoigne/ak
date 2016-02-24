#include "multibootimpl.h"

#include <stdlib.h>
#include <string.h>

#include <arch/hwinfo.h>
#include <arch/x86/mem/framealloc.h>
#include <arch/x86/mem/map.h>
#include <arch/x86/mem/mmu.h>
#include <arch/x86/mem/brk.h>
#include <kernel/panic.h>
#include <kernel/initrd.h>

bool multiboot_validate(uint32_t magic, paddr_t mb_info) {
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		PANIC("bad multiboot magic %i\n", magic);
	}

	return true;
}

static void vmmap(vaddr_t start, vaddr_t end) {
	pg_map_sequence(KBSSTOPHYS(start), (vaddr_t)start, (size_t)(end - start + PAGE_SIZE));
	task_brk(CurrentTask, end);
	extern void *__curbrk;
	__curbrk = (void *)CurrentTask->brk;
}

void multiboot_mmap(paddr_t mb_info_phys) {
	vaddr_t mb_info = PHYSTOKBSS(mb_info_phys);
	vmmap(KTEXT, mb_info + 4);

	size_t mb_size = *((size_t *)mb_info);

	vmmap(KTEXT, mb_info + mb_size);

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

	while (tag->type != MULTIBOOT_TAG_TYPE_END) {
		if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
			struct multiboot_tag_mmap *mmap    = (struct multiboot_tag_mmap *)tag;
			struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *)&mmap->entries;
			for (int i = 0; i < (int)tag->size; i += mmap->entry_size) {
				if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
					frame_clear_range((paddr_t)entry->addr, (paddr_t)entry->addr + (paddr_t)entry->len);
				}

				entry = (struct multiboot_mmap_entry *)((vaddr_t)entry + (vaddr_t)mmap->entry_size);
			}
		}

		if (tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
			struct multiboot_tag_module *mod = (struct multiboot_tag_module *)tag;

			InitrdStart = mod->mod_start;
			InitrdEnd   = mod->mod_end;

			/* In order to load the initrd, we map it into kernel space,
			   then extend the kernel brk to ensure malloc can't access it */
			frame_set_range(InitrdStart, InitrdEnd);
			Initrd = (cpiohdr_t *)PHYSTOKBSS(InitrdStart);
		}

		tag = (struct multiboot_tag *)((vaddr_t)tag + ((tag->size + 7) & (uint32_t)~7));
	}
}
