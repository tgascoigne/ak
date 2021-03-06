#include "task.h"

#include <stdlib.h>

#include <arch/x86/intr/idt.h>
#include <arch/x86/intr/intr.h>
#include <arch/x86/intr/pic.h>
#include <arch/x86/mem/mmu.h>
#include <arch/x86/proc/context.h>
#include <kernel/proc/task.h>
#include <kernel/proc/sched.h>

void task_enter(task_t *task) {
	int ret;
	ret = task_store_context(&CurrentTask->arch);
	if (!ret) {
		CurrentTask = task;
		task_load_context(&CurrentTask->arch);
	}

	/* exec any pending signals */
	task_signal_exec(CurrentTask);
}

task_t *task_fork(void) {
	task_t *clone = task_clone(CurrentTask);
	task_insert(clone);

	int ret;
	ret = task_fork_context(&clone->arch);

	if (CurrentTask == clone) {
		return 0;
	}
	return clone;
}

static void sched_preempt(isrargs_t *args) {
	kern_sched_yield();
}

void task_mask_preempt(void) {
	pic_mask(INT_PIT - INT_PIC1_BASE);
}

void task_unmask_preempt(void) {
	pic_unmask(INT_PIT - INT_PIC1_BASE);
}

void task_enable_preempt(void) {
	task_mask_preempt();
	idt_set_handler(INT_PIT, sched_preempt);
}
