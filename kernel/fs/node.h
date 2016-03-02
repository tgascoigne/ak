#pragma once

#include <kernel/io/stat.h>
#include <kernel/io/dev.h>

#define FS_PATH_DELIM "/"
#define FS_PATH_DELIM_CHR '/'
#define FS_NAME_MAX 128

typedef enum {
	FsModeFile    = 0100000,
	FsModeSymLink = 0120000,
	FsModeDir     = 040000,
} FsMode;

#define FsModeMask(mode, mask) ((mode & mask) == mask)

typedef struct fsnode fsnode_t;

typedef struct fsnode *(*fscreatefunc_t)(struct fsnode *, const char *name);
typedef iodev_t *(*fsopenfunc_t)(struct fsnode *, int flags);
typedef int (*fsstatfunc_t)(struct fsnode *, struct kernel_stat64 *stat);

typedef enum { VfsTypeDir, VfsTypeFile, VfsTypeDev } VfsType;

typedef struct fsnode_ops {
	fscreatefunc_t create;
	fsopenfunc_t open;
	fsstatfunc_t stat;
} fsnode_ops_t;

typedef struct fsnode {
	char name[FS_NAME_MAX];
	VfsType type;
	FsMode mode;
	fsnode_ops_t;

	struct fsnode *parent;
	struct fsnode *first_child;
	struct fsnode *next_sibling;
} fsnode_t;

extern fsnode_t *FSRootNode;

void fs_create_rootfs(void);
fsnode_t *fs_child_iter(fsnode_t *parent, fsnode_t **next_dest);
fsnode_t *fs_locate(fsnode_t *origin, const char *path);
void fs_link_child(fsnode_t *parent, fsnode_t *child);
