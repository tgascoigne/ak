#include "pit.h"

#include <stdio.h>

#include <arch/x86/io.h>
#include <arch/x86/intr/idt.h>

#define PIT_FREQ 200
#define PIT_DIVISOR 1193180 / PIT_FREQ

static void pit_cb(isrargs_t *args) {
	/* nop */
}

void pit_init(void) {
	idt_set_handler(INT_PIT, &pit_cb);
	io_put8(0x43, 0x36);
	io_put8(0x40, (PIT_DIVISOR & 0xFF));
	io_put8(0x40, ((PIT_DIVISOR >> 16) & 0xFF));
}
