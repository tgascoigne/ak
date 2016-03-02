#include "gdt.h"

#include <stdio.h>

typedef struct {
	uint16_t limit;
	vaddr_t base;
} __attribute__((packed)) gdtptr_t;

static gdtptr_t GDTPtr;
static gdtentry_t GDT[1024];

static gdtentryno_t GDTNextEntry = GDT_UDATA_ENTRY + 1;

extern void gdt_load(gdtptr_t *gdt);

void gdt_init() {
	/* set up some identity mapped segments */
	uint8_t granularity = GRAN_KBYTE | GRAN_32BIT;
	uint8_t access_base = SEG_PRESENT | SEG_RW;
	gdt_encode_entry(0, 0, 0, 0, 0);
	gdt_encode_entry(GDT_KCODE_ENTRY, 0, 0xFFFFFFFF, access_base | SEG_TYPE_CODE, granularity);
	gdt_encode_entry(GDT_KDATA_ENTRY, 0, 0xFFFFFFFF, access_base | SEG_TYPE_DATA, granularity);
	gdt_encode_entry(GDT_UCODE_ENTRY, 0, 0xFFFFFFFF, access_base | SEG_TYPE_CODE | (3 << SEG_SHL_RING), granularity);
	gdt_encode_entry(GDT_UDATA_ENTRY, 0, 0xFFFFFFFF, access_base | SEG_TYPE_DATA | (3 << SEG_SHL_RING), granularity);

	gdt_reload();
}

void gdt_encode_entry(gdtentryno_t entryno, vaddr_t base, vaddr_t limit, uint8_t access, uint8_t granularity) {
	gdtentry_t *entry = &GDT[entryno];
	entry->base_low   = (uint16_t)(base & 0xFFFF);
	entry->base_mid   = (uint8_t)((base >> 16) & 0xFF);
	entry->base_high  = (uint8_t)((base >> 24) & 0xFF);

	entry->limit_low   = (uint16_t)(limit & 0xFFFF);
	entry->granularity = (uint8_t)((limit >> 16) & 0x0F);

	entry->granularity |= (uint8_t)(granularity << 4);
	entry->access = access;
}

gdtentryno_t gdt_alloc_entry(void) {
	return GDTNextEntry++;
}

void gdt_reload(void) {
	GDTPtr.limit = sizeof(GDT) - 1;
	GDTPtr.base = (uint32_t)(&GDT);
	gdt_load(&GDTPtr);
}
