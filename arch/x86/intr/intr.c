#include "intr.h"

void intr_enable(void) {
	__asm__ volatile("sti");
}

void intr_disable(void) {
	__asm__ volatile("cli");
}
