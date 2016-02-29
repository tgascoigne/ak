#include "gdt.h"

#include <stdio.h>
#include <stdint.h>

#include <arch/i386/mem/types.h>

/* access byte fields */
#define SEG_PRESENT (1 << 7)
#define SEG_SHL_RING (5)
#define SEG_TYPE_CODE (3 << 3)
#define SEG_TYPE_DATA (2 << 3)
#define SEG_EXPAND_DOWN (1 << 2)
#define SEG_CONFORMING (1 << 2)
#define SEG_RW (1 << 1)
#define SEG_ACCESSED (1 << 0)

/* granularity byte fields */
#define GRAN_KBYTE (1 << 3)
#define GRAN_32BIT (1 << 2)
#define GRAN_OPAQUE (1 << 0)

/* system segment/gate types */
#define SYS_286_AVAIL_TSS 1
#define SYS_LDT 2
#define SYS_286_BUSY_TSS 3
#define SYS_CALL_GATE 4
#define SYS_TASK_GATE 5
#define SYS_286_INT_GATE 6
#define SYS_286_TRAP_GATE 7
#define SYS_386_AVAIL_TSS 8
#define SYS_386_BUSY_TSS 0xB
#define SYS_386_CALL_GATE 0xC
#define SYS_386_INT_GATE 0xE
#define SYS_386_TRAP_GATE 0xF

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) gdtentry_t;

typedef struct {
	uint16_t limit;
	vaddr_t base;
} __attribute__((packed)) gdtptr_t;

static gdtptr_t GDTPtr;
static gdtentry_t GDT[5];

extern void gdt_load(gdtptr_t *gdt);
static void gdt_encode_entry(gdtentry_t *entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void gdt_init() {
	/* set up some identity mapped segments */
	uint8_t granularity = GRAN_KBYTE | GRAN_32BIT;
	uint8_t access_base = SEG_PRESENT | SEG_RW;
	gdt_encode_entry(&GDT[0], 0, 0, 0, 0);
	gdt_encode_entry(&GDT[1], 0, 0xFFFFFFFF, access_base | SEG_TYPE_CODE, granularity);
	gdt_encode_entry(&GDT[2], 0, 0xFFFFFFFF, access_base | SEG_TYPE_DATA, granularity);
	gdt_encode_entry(&GDT[3], 0, 0xFFFFFFFF, access_base | SEG_TYPE_CODE | (3 << SEG_SHL_RING), granularity);
	gdt_encode_entry(&GDT[4], 0, 0xFFFFFFFF, access_base | SEG_TYPE_DATA | (3 << SEG_SHL_RING), granularity);

	/* load the gdt */
	GDTPtr.limit = sizeof(GDT) - 1;
	GDTPtr.base = (uint32_t)(&GDT);
	gdt_load(&GDTPtr);
}

static void gdt_encode_entry(gdtentry_t *entry, vaddr_t base, vaddr_t limit, uint8_t access, uint8_t granularity) {
	entry->base_low  = (uint16_t)(base & 0xFFFF);
	entry->base_mid  = (uint8_t)((base >> 16) & 0xFF);
	entry->base_high = (uint8_t)((base >> 24) & 0xFF);

	entry->limit_low   = (uint16_t)(limit & 0xFFFF);
	entry->granularity = (uint8_t)((limit >> 16) & 0x0F);

	entry->granularity |= (uint8_t)(granularity << 4);
	entry->access = access;
}
