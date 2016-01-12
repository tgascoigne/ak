#include "idt.h"

#include <string.h>

#include <kernel/panic.h>

#define NUM_INTERRUPTS 256

static char* InterruptDescriptions[] = {
    "Divide error", "Debug exceptions",    "Nonmaskable interrupt",     "Breakpoint",	 "Overflow",
    "Bounds check", "Invalid opcode",      "Coprocessor not available", "Double fault",       "(reserved)",
    "Invalid TSS",  "Segment not present", "Stack exception",		"General protection", "Page fault",
    "(reserved)",   "Coprocessor error",
};

static idtdescr_t Idt[NUM_INTERRUPTS];
static idtptr_t IdtPtr;

/* handler callbacks for each interrupt - routed by idt_handler */
static isrfunc_t IdtCallbacks[NUM_INTERRUPTS];

/* root (asm) entry points for interrupts */
extern void* IsrRoots[NUM_INTERRUPTS];

void idt_init(void) {
	/* find the kernel's cs */
	uint16_t cs;
	__asm__ volatile("movw %%cs, %0\n\t" : "=r"(cs)::);

	memset(&Idt, 0, sizeof(Idt));
	memset(&IdtCallbacks, 0, sizeof(IdtCallbacks));
	for (int i = 0; i < NUM_INTERRUPTS; i++) {
		idt_encode_addrs(&Idt[i], IsrRoots[i], cs);
		idt_encode_interrupt(&Idt[i]);
	}

	/* load the idt */
	IdtPtr.limit = sizeof(Idt) - 1;
	IdtPtr.base = (uint32_t)(&Idt);
	idt_load(&IdtPtr);
}

void idt_set_handler(uint8_t interrupt, isrfunc_t func) {
	IdtCallbacks[interrupt] = func;
}

void idt_encode_addrs(idtdescr_t* descr, void* offset, uint16_t selector) {
	descr->offset_low = (uint32_t)(offset)&0xFFFF;
	descr->offset_high = ((uint32_t)(offset) >> 16) & 0xFFFF;
	descr->selector = selector & 0xFFFF;
	descr->present = 1;
	descr->zero0 = 0;
}

void idt_encode_interrupt(idtdescr_t* descr) {
	descr->type = 0xE;
}

void idt_encode_trap(idtdescr_t* descr) {
	descr->type = 0xF;
}

void idt_handler(isrargs_t* regs) {
	if (IdtCallbacks[regs->int_no] != NULL) {
		IdtCallbacks[regs->int_no](regs);
	} else {
		PANIC("Unhandled interrupt: %s, err: %d\n", InterruptDescriptions[regs->int_no], regs->err_code);
	}
}

void idt_load(idtptr_t* idt) {
	__asm__ volatile("lidt %0" ::"m"(idt));
}
