#include "node.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <kernel/fs/tmpfs.h>
#include <kernel/fs/devfs.h>

fsnode_t *FSRootNode;

void fs_create_rootfs(void) {
	FSRootNode = tmpfs_create("root");
	fs_link_child(FSRootNode, devfs_get());
}

fsnode_t *fs_locate(fsnode_t *origin, const char *path) {
	char *path_buf = malloc(strlen(path) + 1);

	strcpy(path_buf, path);
	char *tok     = strtok(path_buf, FS_PATH_DELIM);
	fsnode_t *cur = origin;
	do {
		// locate the fsnode_t* for the current node name
		fsnode_t *child = NULL;

		while (fs_child_iter(cur, &child)) {
			if (strcmp(tok, child->name) == 0) {
				cur = child;
				break;
			}
		}

		if (child == NULL) {
			// missing child
			free(path_buf);
			return NULL;
		}

		// find the next node name in the path
		tok = strtok(NULL, FS_PATH_DELIM);
	} while (tok != NULL);

	free(path_buf);
	return cur;
}

fsnode_t *fs_child_iter(fsnode_t *parent, fsnode_t **next_dest) {
	if (*next_dest == NULL) {
		*next_dest = parent->first_child;
	} else {
		*next_dest = (*next_dest)->next_sibling;
	}
	return *next_dest;
}

void fs_link_child(fsnode_t *parent, fsnode_t *child) {
	if (parent->first_child == NULL) {
		// parent has no children, this is it's first
		parent->first_child = child;
	} else {
		// parent has children, insert it amongst siblings
		fsnode_t *i = NULL;

		for (i = parent->first_child; i != NULL && strcmp(i->name, child->name) < 0; i = i->next_sibling) {
		}

		if (i == NULL) {
			// replace the first child
			fsnode_t *next      = parent->first_child;
			parent->first_child = child;
			child->next_sibling = next;
		} else {
			// insert into the sorted list
			fsnode_t *next      = i->next_sibling;
			i->next_sibling     = child;
			child->next_sibling = next;
		}
	}
}
