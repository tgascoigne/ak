#include "brk.h"

vaddr_t task_brk(task_t* task, vaddr_t brk) {
	vaddr_t cur = task->brk;

#pragma message("map pages accordingly")

	if (brk == 0) {

	} else if (brk > cur) {
		task->brk = cur;
	} else if (brk < cur) {
		task->brk = cur;
	}

	return task->brk;
}
