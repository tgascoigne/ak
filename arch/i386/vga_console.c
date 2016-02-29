#include "vga_console.h"

#include <stdbool.h>
#include <string.h>

#include <mem/map.h>
#include <arch/i386/io.h>

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

#define BUF_PUTC(state, c) state.buffer[VGA_TEXT_ADDR(state.x, state.y)] = (uint16_t)((state.attrib << 8) | c)

void vga_console_putc(char c) {
	/* handle special characters */
	bool special = false;
	switch (c) {
	case '\n':
		state.x = 0;
		state.y++;
		special = true;
		break;
	case 0x8: /* backspace */
		state.x--;
		BUF_PUTC(state, ' ');
		special = true;
		break;
	case '\r':
		return; /* ignore \r */
	}

	if (!special) {
		/* print the character */
		BUF_PUTC(state, c);
		state.x++;
	}

	/* do scrolling etc. if necessary */
	if (state.x >= state.cols) {
		state.x = 0;
		state.y++;
	}

	if (state.x < 0) {
		state.y--;
		state.x = state.cols - 1;
	}

	if (state.y >= state.rows) {
		vga_console_scroll();
	}

	vga_console_set_cursor(state.x, state.y);
}

void vga_console_clear(void) {
	for (int i = 0; i < (state.rows * state.cols) * 2; i++) {
		state.buffer[i] = ((uint16_t)(state.attrib << 8) | ' ');
	}
}

/* This only scrolls the buffer, not the cursor */
static void vga_console_scroll(void) {
	memmove(state.buffer, state.buffer + state.cols, (size_t)((state.rows * state.cols) * 2));
	state.y--;
}

static void vga_console_set_cursor(int x, int y) {
	int pos = VGA_TEXT_ADDR(x, y);
	io_put8(0x3D4, 0x0F);
	io_put8(0x3D5, (uint8_t)(pos & 0xFF));
	io_put8(0x3D4, 0x0E);
	io_put8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
