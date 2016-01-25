#include "vga_console.h"

#include <stdbool.h>
#include <string.h>

#include <mem/map.h>
#include <arch/x86/io.h>

#define VGA_TEXT_ADDR(x, y) (((y)*state.cols) + (x))

static struct {
	uint16_t *buffer;
	int x, y;
	int rows, cols;
	uint8_t attrib;
} state;

static void vga_console_scroll(void);
static void vga_console_set_cursor(int x, int y);

void vga_console_init(void) {
	state.buffer = (uint16_t *)VGA_TEXT_BASE;

	/* set default buffer size */
	state.rows = 25;
	state.cols = 80;

	/* default white on black attrib */
	state.attrib = 0x7;

	/* init cursor */
	state.x = state.y = 0;

	/* disable the vga cursor */
	vga_console_set_cursor(0xFF, 0xFF);

	/* clear the screen */
	vga_console_clear();
}

void vga_console_puts(const char *str) {
	while (*str != '\0') {
		vga_console_putc(*str);
		str++;
	}
}

void vga_console_putc(char c) {
	/* handle special characters */
	bool special = false;
	switch (c) {
	case '\n':
		state.x = 0;
		state.y++;
		special = true;
		break;
	case '\r':
		return; /* ignore \r */
	}

	if (!special) {
		/* print the character */
		int buffer_pos	   = (state.y * state.cols) + state.x;
		state.buffer[buffer_pos] = (uint16_t)((state.attrib << 8) | c);
		state.x++;
	}

	/* do scrolling etc. if necessary */
	if (state.x >= state.cols) {
		state.x = 0;
		state.y++;
	}
	if (state.y >= state.rows) {
		vga_console_scroll();
	}
}

void vga_console_clear(void) {
	memset(state.buffer, 0, (size_t)(state.rows * state.cols) * 2);
}

/* This only scrolls the buffer, not the cursor */
static void vga_console_scroll(void) {
	memmove(state.buffer, state.buffer + state.cols, (size_t)((state.rows * state.cols) * 2));
	state.y--;
}

static void vga_console_set_cursor(int x, int y) {
	int pos = VGA_TEXT_ADDR(x, y);
	io_put8(0x3D4, 0x0F);
	io_put8(0x3D5, (pos & 0xFF));
	io_put8(0x3D4, 0x0E);
	io_put8(0x3D5, ((pos >> 8) & 0xFF));
}
