#pragma once

#include <stdio.h>

#define PANIC(...)           \
	{                        \
		printf(__VA_ARGS__); \
		int hlt = 1;         \
		while (hlt) {        \
		}                    \
	}
