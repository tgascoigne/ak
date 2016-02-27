#include "exec.h"

#include <arch/i386/mem/mmu.h>

static int arg_count(char *const arr[]) {
	int n = 0;
	for (; arr[n] != NULL; n++) {
	}
	return n;
}

int arch_exec(startfunc_t start, char *const argv[], char *const envp[]) {

#pragma message "need a TLS segment set up.. for now just map 0x0"
	pg_alloc(0);

	int envc = arg_count(envp);
	int argc = arg_count(argv);

	void **stack = (void **)malloc((unsigned int)(envc + argc + 4));
	int sp       = 0;
	stack[sp++] = NULL;
	for (int i = envc - 1; i >= 0; i--) {
		stack[sp++] = envp[i];
	}
	stack[sp++] = NULL;
	for (int i = argc - 1; i >= 0; i--) {
		stack[sp++] = argv[i];
	}
	stack[sp++] = (void *)argc;

	__asm__ volatile("movl $0,%%eax\n\t"
	                 "movl %[sp], %%ecx\n\t"
	                 "movl %[stack], %%edx\n\t"
	                 "sp_loop:\n\t"
	                 "pushl (%%edx)\n\t"
	                 "add $4,%%edx\n\t"
	                 "subl $1,%%ecx\n\t"
	                 "cmpl %%ecx,%%eax\n\t"
	                 "jnz sp_loop\n\t"
	                 "movl $0, %%edx\n\t"
	                 "sti\n\t"
	                 "jmpl  *%[start]\n\t"
	                 :
	                 : [start] "b"(start), [stack] "d"(stack), [sp] "c"(sp));

#pragma message "extract exit code, clean up stack"
	return 0;
}
