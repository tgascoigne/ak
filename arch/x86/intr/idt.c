#include "idt.h"

#include <string.h>

#include <arch/x86/intr/pic.h>
#include <kernel/panic.h>

#define NUM_INTERRUPTS 256

static char *InterruptDescriptions[] __attribute__((used)) = {
    "Divide error", "Debug exceptions",    "Nonmaskable interrupt",     "Breakpoint",	 "Overflow",
    "Bounds check", "Invalid opcode",      "Coprocessor not available", "Double fault",       "(reserved)",
    "Invalid TSS",  "Segment not present", "Stack exception",		"General protection", "Page fault",
    "(reserved)",   "Coprocessor error",
};

static idtdescr_t IDT[NUM_INTERRUPTS];
static idtptr_t IDTPtr;

/* handler callbacks for each interrupt - routed by idt_handler */
static isrfunc_t IDTCallbacks[NUM_INTERRUPTS];

/* root (asm) entry points for interrupts */
extern void *ISRRoots[NUM_INTERRUPTS];

#include <arch/x86/mem/map.h>

void idt_init(void) {
	/* find the kernel's cs */
	uint16_t cs;
	__asm__ volatile("movw %%cs, %0\n\t" : "=r"(cs)::);

	memset(&IDT, 0, sizeof(IDT));
	memset(&IDTCallbacks, 0, sizeof(IDTCallbacks));
	for (int i = 0; i < NUM_INTERRUPTS; i++) {
		idt_encode_addrs(&IDT[i], (vaddr_t)ISRRoots[i], cs);
		idt_encode_interrupt(&IDT[i]);
	}

	/* load the idt */
	IDTPtr.limit = sizeof(IDT) - 1;
	IDTPtr.base = (vaddr_t)&IDT;
	idt_load(IDTPtr);

	pic_init();
}

void idt_set_handler(uint8_t interrupt, isrfunc_t func) {
	IDTCallbacks[interrupt] = func;
}

void idt_encode_addrs(idtdescr_t *descr, vaddr_t offset, uint16_t selector) {
	descr->offset_low  = offset & 0xFFFF;
	descr->offset_high = (offset >> 16) & 0xFFFF;
	descr->selector    = selector & 0xFFFF;
	descr->present     = 1;
	descr->zero0       = 0;
}

void idt_encode_interrupt(idtdescr_t *descr) {
	descr->type = 0xE;
}

void idt_encode_trap(idtdescr_t *descr) {
	descr->type = 0xF;
}

void idt_handler(isrargs_t *regs) {
	if (regs->int_no >= INT_PIC1_BASE && regs->int_no < INT_PIC_END) {
		pic_eoi(regs);
	}

	if (IDTCallbacks[regs->int_no] != NULL) {
		IDTCallbacks[regs->int_no](regs);
	} else {
		printf("Unhandled interrupt <%x>, err: %d\n", regs->int_no, regs->err_code);
	}
}

void idt_load(idtptr_t idt) {
	__asm__ volatile("lidt %0" ::"m"(idt));
}

void idt_nop_handler(isrargs_t *x) {
}
