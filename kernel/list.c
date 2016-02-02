#include "list.h"

#include <stddef.h>

void *list_prev(list_head_t *elem) {
	return elem->prev;
}

void *list_next(list_head_t *elem) {
	return elem->next;
}

void list_insert_after(list_head_t *after, list_head_t *elem) {
	list_head_t *oldnext = after->next;

	if (after->next == NULL && after->prev == NULL) {
		after->next = after->prev = elem;
		elem->next = elem->prev = after;
	} else {
		after->next   = elem;
		oldnext->prev = elem;
		elem->next    = oldnext;
		elem->prev    = after;
	}
}

void list_insert_before(list_head_t *before, list_head_t *elem) {
	list_insert_after(before->prev, elem);
}
