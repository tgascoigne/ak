#include "console.h"

#include <stdbool.h>
#include <string.h>

#include <arch/x86/vga_console.h>
#include <kernel/module.h>
#include <kernel/io/dev.h>
#include <kernel/io/tty.h>
#include <kernel/io/fdio.h>

static int console_open(iodev_t *dev, int flags) {
	return 0;
}

static ssize_t console_read(iodev_t *dev, void *buf, size_t count) {
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

static int console_close(iodev_t *dev) {
	return 0;
}

static iodev_t ConsoleDev = {
    .open = console_open, .read = console_read, .write = console_write, .close = console_close,
};

static bool console_register(void) {
	vga_console_init();

	KConsole = &ConsoleDev;

	/* create stdin, stdout, stderr */
	fd_open(KConsole);
	fd_open(KConsole);
	fd_open(KConsole);

	return true;
}

module_init_prio(console_register, MODINIT_TTY);
