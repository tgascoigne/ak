#include "pic.h"

#include <arch/i386/io.h>

#define ICW1_ICW4 0x1
#define ICW1_INIT 0x10
#define ICW4_8086 0x1

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20

void pic_init(void) {
	pic_remap();
}

void pic_remap(void) {
	uint8_t m1, m2;
	m1 = io_get8(PIC1_DATA);
	m2 = io_get8(PIC2_DATA);

	io_put8(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
	io_put8(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);

	io_put8(PIC1_DATA, INT_PIC1_BASE);
	io_put8(PIC2_DATA, INT_PIC2_BASE);

	io_put8(PIC1_DATA, 0x04);
	io_put8(PIC2_DATA, 0x02);

	io_put8(PIC1_DATA, ICW4_8086);
	io_put8(PIC2_DATA, ICW4_8086);

	io_put8(PIC1_DATA, m1);
	io_put8(PIC2_DATA, m2);
}

void pic_eoi(isrargs_t *args) {
	if (args->int_no > INT_PIC2_BASE) {
		io_put8(PIC2_COMMAND, PIC_EOI);
	}
	io_put8(PIC1_COMMAND, PIC_EOI);
}

void pic_mask(uint8_t irq) {
	uint16_t port;
	uint8_t val;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}

	val = (uint8_t)io_get8(port) | (uint8_t)(1 << irq);
	io_put8(port, val);
}

void pic_unmask(uint8_t irq) {
	uint16_t port;
	uint8_t val;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}

	val = io_get8(port) & ~(1 << irq);
	io_put8(port, val);
}
