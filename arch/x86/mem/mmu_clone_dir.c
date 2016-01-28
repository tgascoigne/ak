#include "mmu.h"

#include <string.h>

#include <arch/x86/mem/framealloc.h>

static pgentry_t pg_clone_tbl(pgentry_t tbl);
static pgentry_t pg_clone_page(pgentry_t page);
static pgentry_t pg_clone_page_4m(pgentry_t page);
static void frame_copy(paddr_t src, paddr_t dest);

paddr_t pg_clone_dir(pgentry_t dirframe) {
	paddr_t newdirframe = pg_dir_new();

	pgentry_t *dir    = pg_tmp_map((pgaddr_t)dirframe);
	pgentry_t *newdir = pg_tmp_map((pgaddr_t)newdirframe);

	for (int i = 0; i < 1024; i++) {
		if (dir[i] != NilPgEnt) {
			if ((dir[i] & PAGE_EXTENDED) != 0) {
				newdir[i] = pg_clone_page_4m(dir[i]);
			} else {
				newdir[i] = pg_clone_tbl(dir[i]);
			}
		}
	}

	pg_tmp_unmap(dir);
	pg_tmp_unmap(newdir);
	return newdirframe;
}

static pgentry_t pg_clone_tbl(pgentry_t tblent) {
	uint32_t flags   = (uint32_t)PTE_FLAGS(tblent);
	paddr_t tblframe = (paddr_t)PTE_ADDR(tblent);

	if ((flags & PAGE_LINK) != 0) {
		return tblent;
	}

	if ((flags & PAGE_PRESENT) == 0) {
		// Table not present, but some flags are set
		return tblent;
	}

	paddr_t newtblframe = pg_dir_new();

	pgentry_t *tbl    = pg_tmp_map((pgaddr_t)tblframe);
	pgentry_t *newtbl = pg_tmp_map((pgaddr_t)newtblframe);

	for (int i = 0; i < 1024; i++) {
		if (tbl[i] != NilPgEnt) {
			newtbl[i] = pg_clone_page(tbl[i]);
		}
	}

	pg_tmp_unmap(tbl);
	pg_tmp_unmap(newtbl);
	return PAGE_ENTRY(newtblframe, flags);
}

static pgentry_t pg_clone_page(pgentry_t page) {
	uint32_t flags  = (uint32_t)PTE_FLAGS(page);
	paddr_t pgframe = (paddr_t)PTE_ADDR(page);

	if ((flags & PAGE_PRESENT) == 0) {
		// Page not present, but some flags are set
		return page;
	}

	if ((flags & PAGE_LINK) != 0) {
		return page;
	}

	paddr_t newpgframe = frame_alloc();
	frame_set(newpgframe);
	frame_copy(pgframe, newpgframe);

	return PAGE_ENTRY(newpgframe, flags);
}

static pgentry_t pg_clone_page_4m(pgentry_t page) {
	uint32_t flags  = (uint32_t)PDE_FLAGS(page);
	paddr_t pgframe = (paddr_t)PDE_ADDR(page);

	if ((flags & PAGE_PRESENT) == 0) {
		// Page not present, but some flags are set
		return page;
	}

	if ((flags & PAGE_LINK) != 0) {
		return page;
	}

	paddr_t newpgframe = frame_alloc_4m();
	int num_pages      = 1024;
	paddr_t limit = newpgframe + (paddr_t)(num_pages * PAGE_SIZE);
	frame_set_range(newpgframe, limit);
	for (int i = 0; i < num_pages; i++) {
		paddr_t offset = (paddr_t)i * PAGE_SIZE;
		frame_copy(pgframe + offset, newpgframe + offset);
	}

	return PAGE_ENTRY(newpgframe, flags);
}

static void frame_copy(paddr_t src, paddr_t dest) {
	const char *vsrc = pg_tmp_map(src);
	char *vdest = pg_tmp_map(dest);
	memcpy(vdest, vsrc, PAGE_SIZE);
	pg_tmp_unmap(vsrc);
	pg_tmp_unmap(vdest);
}
