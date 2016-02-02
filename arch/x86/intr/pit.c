#include "pit.h"

#include <stdio.h>

#include <arch/x86/io.h>
#include <arch/x86/intr/idt.h>

static uint32_t const pit_freq    = 200;
static uint32_t const pit_divisor = 1193180 / pit_freq;

static void pit_cb(isrargs_t *args) {
	/* nop */
}

void pit_init(void) {
	idt_set_handler(INT_PIT, &pit_cb);
	io_put8(0x43, 0x36);
	io_put8(0x40, (pit_divisor & 0xFF));
	io_put8(0x40, ((pit_divisor >> 16) & 0xFF));
}
