#include "console.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arch/i386/vga_console.h>
#include <arch/i386/kbd.h>
#include <kernel/module.h>
#include <kernel/io/dev.h>
#include <kernel/io/fdio.h>
#include <kernel/proc/task.h>
#include <kernel/fs/devfs.h>
#include <intr/intr.h>

#define CONS_RBUF_LEN 256
static char ConsoleReadBuf[CONS_RBUF_LEN];
static int RBufRead  = 0;
static int RBufWrite = 0;

static int console_open(iodev_t *dev, int flags) {
#pragma message("query vga to initialize rows, cols")
	return 0;
}

static ssize_t console_write(iodev_t *dev, const void *buf, size_t count) {
	const char *cbuf = (const char *)buf;
	size_t i;
	for (i = 0; i < count; i++) {
		vga_console_putc(*cbuf++);
	}
	return (ssize_t)i;
}

static void console_rbuf_putc(char c) {
	ConsoleReadBuf[RBufWrite++ % CONS_RBUF_LEN] = c;
}

static ssize_t console_rbuf_getc(char *buf) {
	if (RBufRead < RBufWrite) {
		*buf = ConsoleReadBuf[RBufRead++ % CONS_RBUF_LEN];
		return 1;
	}
	return 0;
}

static ssize_t console_read(iodev_t *dev, void *buf, size_t count) {
	intr_enable();

	char *cbuf = (char *)buf;
	ssize_t c = 0;
	while (1) {
		if (console_rbuf_getc(&cbuf[c])) {
			if (cbuf[c] == 0x08) { /* backspace */
				if (c > 0) {
					vga_console_putc(cbuf[c]);
					cbuf[c] = 0x0;
					c--;
				}
				continue;
			}
			vga_console_putc(cbuf[c]);

			if (cbuf[c] == '\n' || cbuf[c] == '\0') {
				return c + 1;
			}

			c++;
		}
	}
	return -1;
}

static int console_close(iodev_t *dev) {
	return 0;
}

static iodev_t ConsoleDev = {
    .open = console_open, .read = console_read, .write = console_write, .close = console_close,
};

static bool console_register(void) {
	vga_console_init();
	kbd_init(console_rbuf_putc);

	fdescr_t *consfd = (fdescr_t *)malloc(sizeof(fdescr_t));
	consfd->dev = &ConsoleDev;
	devfs_register_device("tty", &ConsoleDev);

	return true;
}

module_init_prio(console_register, MODINIT_TTY);
