/*
 * 10/02/2018
 *
 * iterative, has parent, uses balance factor, intrusive
 *
 * Interface to Eric Biggers' AVL tree implementation (see
 * README). <https://github.com/ebiggers/avl_tree>
 */

#include <stddef.h> /* offsetof */

#include "avl_tree.h"

const char *magic_string = "binary_tree_module";

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

struct avl_tree_root {
	struct avl_tree_node *avl_tree_node;
};

#define AVL_ROOT  (struct avl_tree_root) {NULL, }

struct avl_tree_link {
	struct avl_tree_node *parent;
	struct avl_tree_node **node;
};

struct foo {
	struct avl_tree_node node;
	unsigned long key;
};

#define tree_search_for_each(l, current) \
	for ((l)->parent = NULL, (l)->node = current; \
	     *current; \
	     (l)->parent = *(l)->node, (l)->node = current)

static struct foo*
search(struct avl_tree_link *link, struct avl_tree_root *root,
       unsigned long key)
{
	struct avl_tree_node **current = &root->avl_tree_node;

	tree_search_for_each (link, current) {
		struct foo *tmp;

		tmp = container_of(*current, struct foo, node);

		if (key < tmp->key)
			current = &(*current)->left;
		else if (key > tmp->key)
			current = &(*current)->right;
		else
			return tmp;
	}

	return NULL;
}

/*
 * TODO: the use of link in deletion might have some performance
 * penalty. It was done this way to allow reusing search()
 */
static int
avl_delete(struct avl_tree_root *root, unsigned long key)
{
	struct avl_tree_link link;

	if (search(&link, root, key) == NULL)
		return -1;

	avl_tree_remove(&root->avl_tree_node, *link.node);

	return 0;
}

static int
avl_insert(struct avl_tree_root *root, struct foo *new)
{
	struct avl_tree_link link;

	if (search(&link, root, new->key) != NULL)
		return -1;

	*link.node = &new->node;
	(*link.node)->parent_balance = (uintptr_t)link.parent | 1;
	avl_tree_rebalance_after_insert(&root->avl_tree_node, *link.node);

	return 0;
}

size_t
get_root_size(void)
{
	return sizeof(struct avl_tree_root);
}

size_t
get_element_size(void)
{
	return sizeof(struct foo);
}

size_t
get_root_node_offset(void)
{
	return offsetof(struct avl_tree_root, avl_tree_node);
}

size_t
get_left_offset(void)
{
	return offsetof(struct avl_tree_node, left);
}

size_t
get_right_offset(void)
{
	return offsetof(struct avl_tree_node, right);
}

size_t
get_node_offset_in_element(void)
{
	return offsetof(struct foo, node);
}

size_t
get_key_offset_in_element(void)
{
	return offsetof(struct foo, key);
}

unsigned int
get_balance(void *node)
{
	return (((struct avl_tree_node*) node)->parent_balance & 3) - 1;
}

void
insert(void *root, void *pos)
{
	struct foo *new = pos;

	avl_insert(root, new);
}

void
delete(void *root, unsigned long key)
{
	avl_delete(root, key);
}

void
init(void *_root)
{
	struct avl_tree_root *root = _root;

	*root = AVL_ROOT;
}
