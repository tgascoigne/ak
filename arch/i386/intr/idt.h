#pragma once

#include <stdint.h>

#include <arch/i386/mem/map.h>

#define INT_DIV_ERROR 0
#define INT_DEBUG_EXCEPTION 1
#define INT_NONMASKABLE_INTERRUPT 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUNDS_CHECK 5
#define INT_INVALID_OPCODE 6
#define INT_NO_COPROCESSOR 7
#define INT_DOUBLE_FAULT 8
#define INT_INVALID_TSS 10
#define INT_NO_SEGMENT 11
#define INT_STACK_EXCEPTION 12
#define INT_GENERAL_PROTECTION 13
#define INT_PAGE_FAULT 14
#define INT_COPROCESSOR_ERROR 16

#define INT_PIC1_BASE 32
#define INT_PIT (INT_PIC1_BASE + 0)
#define INT_KBD (INT_PIC1_BASE + 1)
#define INT_PIC2_BASE (INT_PIC1_BASE + 8)
#define INT_PIC_END (INT_PIC2_BASE + 8)
#define INT_SYSCALL 128

typedef struct idt_handler_args {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} isrargs_t;

typedef struct __attribute__((packed)) {
	vaddr_t offset_low : 16;
	uint16_t selector : 16;
	uint8_t zero0 : 8;
	uint8_t type : 5;
	uint8_t dpl : 2;
	uint8_t present : 1;
	vaddr_t offset_high : 16;
} idtdescr_t;

typedef struct {
	uint16_t limit;
	vaddr_t base;
} __attribute__((packed)) idtptr_t;

typedef void (*isrfunc_t)(isrargs_t *);

void idt_init(void);
void idt_set_handler(uint8_t interrupt, isrfunc_t func);
void idt_load(idtptr_t idt);
void idt_encode_addrs(idtdescr_t *descr, vaddr_t offset, uint16_t selector);
void idt_encode_interrupt(idtdescr_t *descr);
void idt_encode_trap(idtdescr_t *descr);
void idt_nop_handler(isrargs_t *);
