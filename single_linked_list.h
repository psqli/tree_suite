/*
 * 07/04/2018
 * NULL terminated single linked list
 *
 * from Linux kernel include/linux/llist.h
 *
 * HEAD -> a -> b -> c -> NULL
 */

#ifndef SINGLE_LINKED_LIST_H
#define SINGLE_LINKED_LIST_H

#include <stddef.h> /* offsetof */

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

struct list_node {
	struct list_node *next;
};

struct list_head {
	struct list_node *first;
};

#define LIST_HEAD_INIT  { NULL }

static inline void
list_add(struct list_node *new, struct list_head *head)
{
	new->next = head->first;
	head->first = new;
}

#define list_for_each(pos, node) \
  for ((pos) = (node); pos; (pos) = (pos)->next)

#define list_for_each_safe(pos, n, node) \
  for ((pos) = (node); (pos) && ((n) = (pos)->next, 1); (pos) = (n))

#endif /* SINGLE_LINKED_LIST_H */
