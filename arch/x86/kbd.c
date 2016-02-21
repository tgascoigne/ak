#include "kbd.h"

#include <keymap.h>
#include <intr/idt.h>
#include <io.h>

#define KBD_DATA 0x60
#define KBD_STATUS 0x64
#define KBD_CMD 0x64

static consbufputc_t console_rbuf_putc;

static void kbd_intr(isrargs_t *args) {
	while (io_get8(KBD_STATUS) & 0x1) {
		uint8_t scode = io_get8(KBD_DATA);
		if (scode & 0x80) {
#pragma message("keyup")
			return;
		}

		console_rbuf_putc(Keymap[scode]);
	}
}

void kbd_init(consbufputc_t putc) {
	console_rbuf_putc = putc;
	idt_set_handler(INT_KBD, &kbd_intr);
}
