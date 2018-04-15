/*
 * check whether two trees are identical
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

#include <limits.h> /* UINT_MAX */
#include <stdio.h> /* printf */
#include <stdlib.h> /* srandom */
#include <time.h> /* clock_gettime */

#include "tree_manager.h"

/* number of elements to insert in the trees during the test */
#define N_ELEMENTS  1000000

/* maximum size of node stack used in tree_is_identical() */
#define STACK_SIZE  1024

#define get_left(tree, ptr)  tree_node_get_left(tree, ptr)
#define get_right(tree, ptr) tree_node_get_right(tree, ptr)
#define get_key(tree, ptr)  tree_node_get_key(tree, ptr)

/*
 * based on:
 * <https://www.geeksforgeeks.org/
 *  iterative-function-check-two-trees-identical>
 * and
 * <http://www.techiedelight.com/
 *  check-if-two-binary-trees-are-identical-not-iterative-recursive>
 */
int /* NOTE: boolean function */
tree_is_identical(struct tree_info *a, void *_root_a,
                  struct tree_info *b, void *_root_b)
{
	void *root_a, *root_b;
	void *stack_a[STACK_SIZE];
	void *stack_b[STACK_SIZE];
	unsigned long idx = 0;

	/* NOTE: debug */
	unsigned long max_idx = 0, n_checked = 0;

	root_a = tree_root_get_node(a, _root_a);
	root_b = tree_root_get_node(b, _root_b);

	/* identical if both trees are empty */
	if (!root_a && !root_b)
		return 1;

	/* not identical if one is empty and other is not */
	if (!root_a || !root_b)
		return 0;

	/* push roots */
	stack_a[idx] = root_a;
	stack_b[idx++] = root_b;

	while (idx) {
		/* NOTE: debug */
		n_checked++;
		if (idx > max_idx)
			max_idx = idx;

		/* get top nodes and compare them */
		void *node_a = stack_a[idx - 1];
		void *node_b = stack_b[idx - 1];

		if (get_key(a, node_a) != get_key(b, node_b)) {
			printf("keys differ a=%ld b=%ld\n",
			       get_key(a, node_a),
			       get_key(b, node_b));
			return 0;
		}

		/*
		 * corresponding nodes are compared
		 * only once. pop them from stack
		 */
		idx--;

		/*
		 * if corresponding children are not
		 * null push them to the stack
		 */

		if (get_left(a, node_a) && get_left(b, node_b)) {
			stack_a[idx] = get_left(a, node_a);
			stack_b[idx++] = get_left(b, node_b);
			if (idx == STACK_SIZE)
				return -1;
		} else if (get_left(a, node_a) || get_left(b, node_b)) {
			/* one left child is empty and other is not */
			return 0;
		}

		if (get_right(a, node_a) && get_right(b, node_b)) {
			stack_a[idx] = get_right(a, node_a);
			stack_b[idx++] = get_right(b, node_b);
			if (idx == STACK_SIZE)
				return -1;
		} else if (get_right(a, node_a) || get_right(b, node_b)) {
			/* one right child is empty and other is not */
			return 0;
		}
	}

	/* NOTE: debug */
	printf("maximum stack size = %ld\n", max_idx);
	printf("total elements checked = %ld\n", n_checked);

	return 1;
}

static void
__main(struct tree_operations *ops_a, struct tree_operations *ops_b)
{
	struct tree_info tree_info_a;
	struct tree_memory tree_memory_a;
	struct tree_info tree_info_b;
	struct tree_memory tree_memory_b;
	unsigned long i;

	/* set up tree a */
	tree_info_setup(&tree_info_a, ops_a);
	tree_memory_allocate(&tree_memory_a, &tree_info_a, N_ELEMENTS);
	ops_a->init(tree_memory_a.root);

	/* set up tree b */
	tree_info_setup(&tree_info_b, ops_b);
	tree_memory_allocate(&tree_memory_b, &tree_info_b, N_ELEMENTS);
	ops_b->init(tree_memory_b.root);

	tree_fill_in_order(&tree_memory_a, &tree_info_a, N_ELEMENTS);
	tree_randomize(&tree_memory_a, &tree_info_a, N_ELEMENTS);
	/* copy keys from tree_a to tree_b */
	tree_copy_keys(&tree_memory_b, &tree_info_b,
	               &tree_memory_a, &tree_info_a, N_ELEMENTS);

	for (i = 0; i < N_ELEMENTS; i++) {
		tree_insert(&tree_memory_a, &tree_info_a, i);
		tree_insert(&tree_memory_b, &tree_info_b, i);
	}

	switch (tree_is_identical(&tree_info_a, tree_memory_a.root,
	                          &tree_info_b, tree_memory_b.root)) {
	case 0:
		printf("not identical\n");
		break;
	case 1:
		printf("identical\n");
		break;
	default: /* -1 */
		printf("error. Probably going to stack overflow\n");
	}

	/* NOTE: deleting elements from trees is a waste of time */

	tree_memory_free(&tree_memory_a);
	tree_memory_free(&tree_memory_b);
}

int
main(int argc, char **argv)
{
	struct list_head tree_list_head = LIST_HEAD_INIT;
	struct tree_library *lib_a, *lib_b;
	struct timespec time_seed;

	if (argc < 3) {
		printf("usage: cmd <library_a> <library_b>\n");
		return 1;
	}

	/* load trees */
	lib_a = tree_library_load(argv[1], &tree_list_head);
	lib_b = tree_library_load(argv[2], &tree_list_head);

	if (!lib_a || !lib_b)
		goto _go_unload_trees;

	printf("%s x %s\n", lib_a->name, lib_b->name);

	/* NOTE: should it be inside tree interface? */
	/* set random() seed */
	clock_gettime(CLOCK_REALTIME, &time_seed);
	srandom(time_seed.tv_nsec % UINT_MAX);

	__main(&lib_a->ops, &lib_b->ops);

_go_unload_trees:
	tree_manager_unload_trees(&tree_list_head);

	return 0;
}
