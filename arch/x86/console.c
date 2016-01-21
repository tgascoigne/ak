#include "console.h"

#include <stdbool.h>

#include <arch/x86/vga_console.h>

static bool initialized = false;

static int console_open(iodev_t *dev, int flags) {
	if (!initialized) {
		vga_console_init();
		initialized = true;
	}
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

iodev_t ConsoleDev = {
    .open = console_open, .read = console_read, .write = console_write, .close = console_close,
};
