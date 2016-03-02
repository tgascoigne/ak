#pragma once

#include <kernel/syscall/thread.h>

int arch_get_thread_area(struct user_desc *u_info);
int arch_set_thread_area(struct user_desc *u_info);
