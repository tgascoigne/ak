#include "io.h"

void io_put8(uint16_t port, uint8_t value) {
	__asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void io_put16(uint16_t port, uint16_t value) {
	__asm__ volatile("outw %1, %0" : : "dN"(port), "a"(value));
}

uint8_t io_get8(uint16_t port) {
	uint8_t value = 0;
	__asm__ volatile("inb %1, %0" : "=a"(value) : "dN"(port));
	return value;
}

uint16_t io_get16(uint16_t port) {
	uint16_t value = 0;
	__asm__ volatile("inw %1, %0" : "=a"(value) : "dN"(port));
	return value;
}
