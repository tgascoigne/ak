#include "read_elf.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <kernel/assert.h>

static int elf_read_header(elf_t *elf);
static int elf_read_shdr(elf_t *elf);
static int elf_read_phdr(elf_t *elf);
static int elf_read_symtab(elf_t *elf);

int elf_read(const char *elf_file, elf_t *elf) {
	/* get the elf file size */
	struct stat fstat;
	int ret;
	ret = stat(elf_file, &fstat);
	kassert(ret == 0, ELF_NO_FILE);

	/* allocate space for it */
	elf->elf_data   = (char *)mmap(0, (size_t)fstat.st_size, 0, MAP_ANONYMOUS, 0, 0);
	elf->elf_length = (size_t)fstat.st_size;

	/* read it from disk */
	FILE *elf_fd = fopen(elf_file, "r");
	kassert(elf_fd != 0, ELF_NO_FILE);
	fseek(elf_fd, 0, SEEK_SET);
	ret = (int)fread(elf->elf_data, 1, elf->elf_length, elf_fd);
	kassert(ret == (int)elf->elf_length, ELF_NO_FILE);
	fclose(elf_fd);

	/* parse it */
	ret = elf_read_header(elf);
	kassert(ret == 0, ret);
	ret = elf_read_shdr(elf);
	kassert(ret == 0, ret);
	ret = elf_read_symtab(elf);
	kassert(ret == 0, ret);
	ret = elf_read_phdr(elf);
	kassert(ret == 0, ret);

	return 1;
}

void elf_free(elf_t *elf) {
	free(elf->elf_data);
}

void elf_print_sections(elf_t *elf) {
	int ret = 0;
	char sh_name[32];
	for (int i = 1; i < elf->num_sections; i++) {
		ret = elf_get_section_name(elf, i, sh_name);
		if (ret == 0) {
			printf("section %d: %s %x\n", i, sh_name, elf->shdr[i].sh_offset);
		} else {
			printf("section %d: error\n", i);
		}
	}
}

void elf_print_segments(elf_t *elf) {
	for (int i = 0; i < elf->num_segments; i++) {
		uint64_t end_addr = elf->phdr[i].p_vaddr + elf->phdr[i].p_memsz;
		printf("segment %2d: 0x%x-0x%llx\n", i, elf->phdr[i].p_vaddr, end_addr);
	}
}

void elf_print_symbols(elf_t *elf) {
	int ret = 0;
	char sym_name[32];
	char sym_type[32];
	for (int i = 1; i < elf->num_symbols; i++) {
		ret               = elf_get_symbol_name(elf, i, sym_name);
		Elf32_Sym *symbol = &elf->symtab[i];

		/* fill in a blank name */
		if (strcmp(sym_name, "") == 0) {
			sprintf(sym_name, "no name");
		}

		/* convert the type to a string */
		switch (ELF64_ST_TYPE(symbol->st_info)) {
		case STT_NOTYPE:
			sprintf(sym_type, "no type");
			break;
		case STT_OBJECT:
			sprintf(sym_type, "object");
			break;
		case STT_FUNC:
			sprintf(sym_type, "function");
			break;
		case STT_SECTION:
			sprintf(sym_type, "section");
			break;
		case STT_FILE:
			sprintf(sym_type, "file");
			break;
		}

		/* find it's virtual and file addr */
		Elf32_Addr vaddr = symbol->st_value;
		uint64_t faddr = 0;
		elf_map_vaddr(elf, vaddr, &faddr); /* ignore mapping errors - dynamic linking etc. */

		if (ret == 0) {
			printf("symbol %3d: %-10s %-10s vaddr: %x faddr: %llx\n", i, sym_type, sym_name, vaddr, faddr);
		} else {
			printf("symbol %d: error\n", i);
		}
	}
}

int elf_map_vaddr(elf_t *elf, Elf32_Addr vaddr, uint64_t *faddr) {
	/* try to find the segment which contains the virtual address */
	for (int i = 1; i < elf->num_segments; i++) {
		Elf32_Phdr *segment  = &elf->phdr[i];
		Elf32_Addr base_addr = segment->p_vaddr;
		Elf32_Addr lim_addr = segment->p_vaddr + elf->phdr[i].p_memsz;
		if (vaddr > base_addr && vaddr < lim_addr) {
			/* Success! */
			Elf32_Addr offset_addr = vaddr - base_addr;
			*faddr                 = segment->p_offset + offset_addr;
			return 0;
		}
	}
	return ELF_UNMAPPED;
}

int elf_map_vaddr_section(elf_t *elf, Elf32_Addr vaddr, int *out) {
	/* try to find the section which contains the virtual address */
	for (int i = 1; i < elf->num_sections; i++) {
		Elf32_Shdr *section  = &elf->shdr[i];
		Elf32_Addr base_addr = section->sh_addr;
		Elf32_Addr lim_addr = section->sh_addr + section->sh_size;
		if (vaddr > base_addr && vaddr < lim_addr) {
			/* Success! */
			*out = i;
			return 0;
		}
	}
	return ELF_UNMAPPED;
}

static int elf_read_header(elf_t *elf) {
	elf->header = (Elf32_Ehdr *)elf->elf_data;
	kassert(elf->header->e_ident[EI_MAG0] == ELFMAG0, ELF_INVALID);
	kassert(elf->header->e_ident[EI_MAG1] == ELFMAG1, ELF_INVALID);
	kassert(elf->header->e_ident[EI_MAG2] == ELFMAG2, ELF_INVALID);
	kassert(elf->header->e_ident[EI_MAG3] == ELFMAG3, ELF_INVALID);
	kassert(elf->header->e_ident[EI_CLASS] == ELFCLASS32, ELF_INVALID_EXECUTABLE);
	kassert(elf->header->e_type == ET_EXEC, ELF_INVALID_EXECUTABLE);
	return 0;
}

static int elf_read_shdr(elf_t *elf) {
	elf->shdr         = (Elf32_Shdr *)(elf->elf_data + elf->header->e_shoff);
	elf->num_sections = elf->header->e_shnum;
	/* locate the common section indices */
	elf->sec_shstrtab = elf->header->e_shstrndx;
	int ret           = 0;
	ret = elf_get_section_by_name(elf, ".strtab", &elf->sec_strtab);
	kassert(ret == 0, ret);
	return 0;
}

static int elf_read_phdr(elf_t *elf) {
	elf->phdr         = (Elf32_Phdr *)(elf->elf_data + elf->header->e_phoff);
	elf->num_segments = elf->header->e_phnum;
	return 0;
}

static int elf_read_symtab(elf_t *elf) {
	int symtab_sec;
	int ret = 0;
	ret = elf_get_section_by_name(elf, ".symtab", &symtab_sec);
	kassert(ret == 0, ret);
	elf->symtab      = (Elf32_Sym *)(elf->elf_data + elf->shdr[symtab_sec].sh_offset);
	elf->num_symbols = elf->shdr[symtab_sec].sh_size / sizeof(Elf32_Sym);
	return 0;
}

int elf_get_section_by_name(elf_t *elf, char *name, int *out) {
	char sec_name[32];
	int ret = 0;
	for (int i = 1; i < elf->num_sections; i++) {
		ret = elf_get_section_name(elf, i, sec_name);
		kassert(ret == 0, ret);
		if (strcmp(sec_name, name) == 0) {
			*out = i;
		}
	}
	return 0;
}

int elf_get_section_name(elf_t *elf, int section, char *out) {
	kassert(section < elf->num_sections, ELF_INVALID_SECTION);
	int strtab_ofs = (int)elf->shdr[section].sh_name;
	int ret = elf_get_strtab_entry(elf, elf->sec_shstrtab, strtab_ofs, out);
	kassert(ret == 0, ret);
	return 0;
}

int elf_get_symbol_faddr(elf_t *elf, int symidx, uint64_t *out) {
	Elf32_Sym *symbol = &elf->symtab[symidx];
	Elf32_Addr vaddr  = symbol->st_value;
	int ret = elf_map_vaddr(elf, vaddr, out);
	kassert(ret == 0, ELF_UNMAPPED);
	return 0;
}

int elf_get_symbol_vaddr(elf_t *elf, int symidx, uint32_t *out) {
	Elf32_Sym *symbol = &elf->symtab[symidx];
	*out              = symbol->st_value;
	return 0;
}

int elf_get_symbol_section(elf_t *elf, int symidx, int *section) {
	Elf32_Sym *symbol = &elf->symtab[symidx];
	Elf32_Addr vaddr  = symbol->st_value;
	int ret = elf_map_vaddr_section(elf, vaddr, section);
	kassert(ret == 0, ELF_UNMAPPED);
	return 0;
}

int elf_get_symbol_by_name(elf_t *elf, char *name, int *out) {
	char sym_name[32];
	for (int i = 0; i < elf->num_symbols; i++) {
		elf_get_symbol_name(elf, i, sym_name); /* ignore errors */
		if (strcmp(sym_name, name) == 0) {
			*out = i;
			return 0;
		}
	}
	return ELF_INVALID_SYMBOL;
}

int elf_get_symbol_name(elf_t *elf, int symbol, char *out) {
	kassert(symbol < elf->num_symbols, ELF_INVALID_SYMBOL);
	int strtab_ofs = (int)elf->symtab[symbol].st_name;
	int ret = elf_get_strtab_entry(elf, elf->sec_strtab, strtab_ofs, out);
	kassert(ret == 0, ret);
	return 0;
}

int elf_get_strtab_entry(elf_t *elf, int strtab, int ofs, char *out) {
	char *shstrtab;
	int ret = elf_get_section_data(elf, strtab, &shstrtab);
	kassert(ret == 0, ELF_NO_SHSTRTAB);

	strcpy(out, shstrtab + ofs);
	return 0;
}

int elf_get_section_data(elf_t *elf, int section, char **out) {
	kassert(section < elf->num_sections, ELF_INVALID_SECTION);
	uint64_t file_ofs = elf->shdr[section].sh_offset;
	*out              = elf->elf_data + file_ofs;
	return 0;
}

int elf_get_section_faddr(elf_t *elf, int section, uint64_t *faddr) {
	kassert(section < elf->num_sections, ELF_INVALID_SECTION);
	*faddr = elf->shdr[section].sh_offset;
	return 0;
}

int elf_get_section_len(elf_t *elf, int section, uint64_t *len) {
	kassert(section < elf->num_sections, ELF_INVALID_SECTION);
	*len = elf->shdr[section].sh_size;
	return 0;
}
