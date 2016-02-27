#pragma once

#define __intr_inline__ static inline __attribute__((always_inline))

__intr_inline__ void intr_enable(void) {
	__asm__ volatile("sti");
}

__intr_inline__ void intr_disable(void) {
	__asm__ volatile("cli");
}
