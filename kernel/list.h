#pragma once

typedef struct list_head { struct list_head *next, *prev; } list_head_t;

#define list_elem(ptr, type, member) ((type *)((char *)(ptr) - (uintptr_t)(&((type *)0)->member)))

void *list_prev(list_head_t *elem);
void *list_next(list_head_t *elem);
void list_insert_after(list_head_t *after, list_head_t *elem);
void list_insert_before(list_head_t *before, list_head_t *elem);
void list_remove(list_head_t *elem);
