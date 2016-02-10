#pragma once

#include <kernel/panic.h>

#define kassert(condition, error_ret)                     \
	if (!(condition)) {                               \
		PANIC("assert failed: %s\n", #condition); \
	}
