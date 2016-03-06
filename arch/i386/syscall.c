#include "syscall.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <arch/i386/intr/idt.h>
#include <arch/i386/kdebug.h>
#include <kernel/syscall/syscall.h>

typedef struct {
    int nargs;
    char *hname;
} syscallmeta_t;

static syscallmeta_t SyscallTbl[] = {
#include <arch/i386/syscallent.h>
};

static void syscall_interrupt(isrargs_t *regs) {
	int syscall     = (int)regs->eax;
	syscall_fn_t fn = syscall_funcs[syscall];
	int argc        = SyscallTbl[syscall].nargs;

	/* syscall arguments are passed in order in the following registers */
	uint32_t all_regs[] = {regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, regs->ebp};

#ifdef _KSTRACE_
    kdebugf("kstrace: <0x%02x> %s(", syscall, SyscallTbl[syscall].hname);
    for (int i = 0; i < argc; i++) {
        kdebugf("0x%02x, ", all_regs[i]);
    }
    kdebugf(")\n");
#endif

	if (fn == NULL) {
		kdebugf("unhandled syscall: %02x\n", syscall);
		return;
	}

	uint32_t ret;
	switch (argc) {
	case 0:
		ret = (fn)();
		break;
	case 1:
		ret = (fn)(all_regs[0]);
		break;
	case 2:
		ret = (fn)(all_regs[0], all_regs[1]);
		break;
	case 3:
		ret = (fn)(all_regs[0], all_regs[1], all_regs[2]);
		break;
	case 4:
		ret = (fn)(all_regs[0], all_regs[1], all_regs[2], all_regs[3]);
		break;
	case 5:
		ret = (fn)(all_regs[0], all_regs[1], all_regs[2], all_regs[3], all_regs[4]);
		break;
	case 6:
		ret = (fn)(all_regs[0], all_regs[1], all_regs[2], all_regs[3], all_regs[4], all_regs[5]);
		break;
	default:
		printf("invalid argc in syscall: %i\n", argc);
		ret = 0;
		break;
	}
	regs->eax = ret;
}

void syscall_arch_init(void) {
	idt_set_handler(INT_SYSCALL, &syscall_interrupt);
}
