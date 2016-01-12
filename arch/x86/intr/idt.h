#pragma once

#include <stdint.h>

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

typedef struct idt_handler_args {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} isrargs_t;

typedef struct __attribute__((packed)) {
	int offset_low : 16;
	int selector : 16;
	int zero0 : 8;
	int type : 5;
	int dpl : 2;
	int present : 1;
	int offset_high : 16;
} idtdescr_t;

typedef struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idtptr_t;

typedef void (*isrfunc_t)(isrargs_t*);

void idt_init(void);
void idt_set_handler(uint8_t interrupt, isrfunc_t func);
void idt_load(idtptr_t* idt);
void idt_encode_addrs(idtdescr_t* descr, void* offset, uint16_t selector);
void idt_encode_interrupt(idtdescr_t* descr);
void idt_encode_trap(idtdescr_t* descr);
