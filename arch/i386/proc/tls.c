#include "tls.h"

#include <mem/gdt.h>

int arch_get_thread_area(struct user_desc *u_info) {
	if ((int)u_info->entry_number == -1) {
		u_info->entry_number = gdt_alloc_entry();
	}

	return 0;
}

int arch_set_thread_area(struct user_desc *u_info) {
	uint8_t granularity = 0;
	uint8_t access_byte = SEG_TYPE_DATA;

	if ((int)u_info->entry_number == -1) {
		u_info->entry_number = gdt_alloc_entry();
	}

	if (u_info->limit_in_pages) {
		granularity |= GRAN_KBYTE;
	}

	if (u_info->seg_32bit) {
		granularity |= GRAN_32BIT;
	}

	if (!u_info->read_exec_only) {
		access_byte |= SEG_RW;
	}

	if (!u_info->seg_not_present) {
		access_byte |= SEG_PRESENT;
	}

	access_byte |= SEG_TYPE_DATA;
	access_byte |= (3 << SEG_SHL_RING);

	gdt_encode_entry((gdtentryno_t)u_info->entry_number, u_info->base_addr, u_info->limit, access_byte, granularity);

	gdt_reload();

	return 0;
}
