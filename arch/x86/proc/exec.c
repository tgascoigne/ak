#include "exec.h"

#include <arch/x86/mem/mmu.h>

static int arg_count(char *const arr[]) {
	int n = 0;
	for (; arr[n] != NULL; n++) {
	}
	return n;
}

int arch_exec(startfunc_t start, char *const argv[], char *const envp[]) {

#pragma message "need a TLS segment set up.. for now just map 0x0"
	pg_alloc(0);

	__asm__ volatile("pushl $0\n\t");
	for (int i = arg_count(envp) - 1; i >= 0; i--) {
		__asm__ volatile("pushl %0\n\t" : : "b"(&envp[i]));
	}

	__asm__ volatile("pushl $0\n\t");
	int argc = arg_count(argv);
	for (int i = argc - 1; i >= 0; i--) {
		__asm__ volatile("pushl %0\n\t" : : "b"(&argv[i]));
	}
	__asm__ volatile("pushl %0\n\t" : : "b"(argc));

	__asm__ volatile("movl $0, %%edx\n\t"
	                 "sti\n\t"
	                 "jmpl  *%0\n\t"
	                 :
	                 : "b"(start));

#pragma message "extract exit code, clean up stack"
	return 0;
}
