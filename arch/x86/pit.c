#include "pit.h"

#include <arch/x86/intr/idt.h>

static void pit_cb(isrargs_t *args) {
	/* nop */
}

void pit_init(void) {
	idt_set_handler(INT_PIT, &pit_cb);
}
