#pragma once

#include <stdio.h>

#define PANIC(...)                   \
	{                            \
		printf(__VA_ARGS__); \
		while (1) {          \
		}                    \
	}
