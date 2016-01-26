#pragma once

#include <stdio.h>

#define PANIC(...) \
	{ printf(__VA_ARGS__); }
