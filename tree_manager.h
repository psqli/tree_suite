/*
 * helpers to manipulate trees
 * Copyright (C) 2018  Ricardo Biehl Pasquali <pasqualirb@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 24/02/2018
 */

#ifndef TREE_MANAGER_H
#define TREE_MANAGER_H

#include "tree_operations.h"
#include "single_linked_list.h"

/*
 * Tree library
 * ============
 */
#if 1 /* tree_library */

struct tree_library {
	struct list_node list_node;

	/* library's name (filename is used) */
	char *name;

	/* dlopen() handle */
	void *library;

	/* has pointers to library's functions */
	struct tree_operations ops;
};

struct tree_library*
tree_library_load(const char *filename, struct list_head *tree_list);

void
tree_manager_unload_trees(struct list_head *tree_list);

int
tree_manager_load_trees(struct list_head *tree_list);

#endif /* tree_library */

/*
 * Tree information
 * ================
 */
#if 1 /* tree_information */

struct tree_info {
	struct tree_operations *ops;

	/* sizes */
	unsigned int root_size;
	unsigned int element_size;

	/* offsets */
	unsigned int root_node_offset;
	unsigned int left_child_offset;
	unsigned int right_child_offset;
	unsigned int node_offset_in_element;
	unsigned int key_offset_in_element;
};

void
tree_info_setup(struct tree_info *info, struct tree_operations *ops);

/*
 * NOTE: we like size of void* to be 1 in pointer arithmetic :-)
 * I hope everything is memory aligned.
 */

#define tree_root_get_node(tree, root) \
	*( (void**) ( (void*) root + (tree)->root_node_offset))

/* ptr refers to the node, not the element */

#define tree_node_get_left(tree, ptr) \
	*( (void**) ( (void*) ptr + (tree)->left_child_offset))

#define tree_node_get_right(tree, ptr) \
	*( (void**) ( (void*) ptr + (tree)->right_child_offset))

#define tree_node_get_key(tree, ptr) \
	*( (unsigned long*) ( (void*) ptr - \
	(tree)->node_offset_in_element + \
	(tree)->key_offset_in_element))

#define tree_node_set_key(tree, ptr, key) \
	*( (unsigned long*) ( (void*) ptr - \
	(tree)->node_offset_in_element + \
	(tree)->key_offset_in_element) ) = key

/* ptr refers to the element */

#define tree_element_get_key(tree, ptr) \
	*( (unsigned long*) ( (void*) ptr + \
	(tree)->key_offset_in_element))

#define tree_element_set_key(tree, ptr, key) \
	*( (unsigned long*) ( (void*) ptr + \
	(tree)->key_offset_in_element) ) = key

#define tree_element_get_node(tree, ptr) \
	*( (void**) ( (void*) ptr + (tree)->node_offset_in_element))

#endif /* tree_information */

/*
 * Tree memory
 * ===========
 */
#if 1 /* tree_memory */

struct tree_memory {
	void *addr;
	void *root;
	void *array;
};

void
tree_memory_free(struct tree_memory *m);

void
tree_memory_allocate(struct tree_memory *m, struct tree_info *i,
                     unsigned int size);

void
tree_fill_in_order(struct tree_memory *m, struct tree_info *i,
                   unsigned long current);

void
tree_randomize(struct tree_memory *m, struct tree_info *i,
               unsigned long current);

void
tree_copy_keys(struct tree_memory *dst_mem,
               struct tree_info *dst_inf,
               struct tree_memory *src_mem,
               struct tree_info *src_inf,
               unsigned long current);

void
tree_assign_keys(struct tree_memory *m, struct tree_info *i,
                 unsigned long *key_array, unsigned long current);

static inline void
tree_delete(struct tree_memory *m, struct tree_info *i, unsigned int idx)
{
	unsigned long *tmp = (void*) m->array + idx * i->element_size +
	                     i->key_offset_in_element;

	i->ops->delete(m->root, *tmp);
}

static inline void
tree_insert(struct tree_memory *m, struct tree_info *i, unsigned int idx)
{
	i->ops->insert(m->root, m->array + idx * i->element_size);
}

#endif /* tree_memory */

#endif /* TREE_MANAGER_H */
