#pragma once

#ifndef ASM_FILE
#include <stdint.h>

#include <arch/i386/mem/types.h>
#endif /* ASM_FILE */

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

#define GDT_KCODE_ENTRY 1
#define GDT_KDATA_ENTRY 2
#define GDT_UCODE_ENTRY 3
#define GDT_UDATA_ENTRY 4
#define GDT_TLS_ENTRY 5

#define GDT_KCODE_OFFS (GDT_KCODE_ENTRY * 8)
#define GDT_KDATA_OFFS (GDT_KDATA_ENTRY * 8)
#define GDT_UCODE_OFFS (GDT_UCODE_ENTRY * 8)
#define GDT_UDATA_OFFS (GDT_UDATA_ENTRY * 8)
#define GDT_TLS_OFFS (GDT_TLS_ENTRY * 8)

#ifndef ASM_FILE
typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} __attribute__((packed)) gdtentry_t;

typedef unsigned int gdtentryno_t;

void gdt_init();
gdtentryno_t gdt_alloc_entry(void);
void gdt_encode_entry(gdtentryno_t entryno, vaddr_t base, vaddr_t limit, uint8_t access, uint8_t granularity);
void gdt_reload(void);
#endif /* ASM_FILE */
