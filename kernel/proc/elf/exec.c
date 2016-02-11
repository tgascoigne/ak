#include "exec.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <mem/mmu.h>

void elf_load(elf_t *elf) {
	for (int i = 0; i < elf->num_segments; i++) {
		Elf32_Phdr *segment = &elf->phdr[i];

		if (segment->p_type != PT_LOAD) {
			continue;
		}

		Elf32_Addr base_addr = segment->p_vaddr;
		Elf32_Addr lim_addr  = segment->p_vaddr + elf->phdr[i].p_memsz;

		bool rw = false;
		if ((segment->p_flags & (PF_R | PF_W))) {
			rw = true;
		}

		pg_alloc_range((vaddr_t)base_addr, (vaddr_t)lim_addr, rw);
		char *seg_data = elf->elf_data + segment->p_offset;
		memcpy((char *)base_addr, (char *)seg_data, segment->p_filesz);
		printf("elf: loaded segment %i into %x-%x\n", i, base_addr, lim_addr);
	}
}

typedef int (*startfunc_t)(int argc, char **argv, char **envp);

void elf_start(elf_t *elf) {
	startfunc_t fn = (startfunc_t)elf->header->e_entry;
	char *prog     = "init";

#pragma message "need a TLS segment set up.. for now just map 0x0"
	pg_alloc(0);

#pragma message "belongs in arch/x86"
	__asm__ volatile("pushl $0\n\t"
			 "pushl $0\n\t"
			 "pushl %0\n\t"
			 "pushl $1\n\t"
			 "jmpl  *%1\n\t"
			 :
			 : "b"(&prog), "b"(fn));
}