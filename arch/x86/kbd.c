#include "kbd.h"

#include <keymap.h>
#include <intr/idt.h>
#include <intr/intr.h>
#include <io.h>

#define KBUF_LEN 256
char KeyboardBuffer[KBUF_LEN];
int KBufRead  = 0;
int KBufWrite = 0;

#define KBD_DATA 0x60
#define KBD_STATUS 0x64
#define KBD_CMD 0x64

static void kbd_intr(isrargs_t *args) {
	while (io_get8(KBD_STATUS) & 0x1) {
		uint8_t scode = io_get8(KBD_DATA);
		if (scode & 0x80) {
#pragma message("keyup")
			return;
		}

		KeyboardBuffer[KBufWrite++ % KBUF_LEN] = Keymap[scode];
	}
}

ssize_t kbd_gets(void *buf, size_t count) {
	intr_enable();
	char *cbuf = (char *)buf;
	while (count > 0 && KBufRead < KBufWrite) {
		*(cbuf++) = KeyboardBuffer[KBufRead++ % KBUF_LEN];
		count--;
	}
	return (ssize_t)(cbuf - (char *)buf);
}

void kbd_init(void) {
	idt_set_handler(INT_KBD, &kbd_intr);
}
