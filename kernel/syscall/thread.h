#pragma once

struct user_desc {
	unsigned int entry_number;
	unsigned long base_addr;
	unsigned int limit;
	unsigned int seg_32bit : 1;
	unsigned int contents : 2;
	unsigned int read_exec_only : 1;
	unsigned int limit_in_pages : 1;
	unsigned int seg_not_present : 1;
	unsigned int useable : 1;
};

void thread_init(void);
int get_thread_area(struct user_desc *u_info);
int set_thread_area(struct user_desc *u_info);
