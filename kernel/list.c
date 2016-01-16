#include "list.h"

void *list_prev(list_head_t *elem) {
	return elem->prev;
}

void *list_next(list_head_t *elem) {
	return elem->next;
}
