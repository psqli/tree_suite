/*
 * prints a tree
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
 * 13/01/2018
 *
 * interactive program to insert, delete and print a tree
 */

#include <alloca.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> /* malloc free */
#include <string.h>
#include <unistd.h> /* read */

#include "tree_manager.h"

#define MAX_STACK_SIZE  64
#define PRINT_HEIGHT  8
#define PRINT_WIDTH  80
#define PRINT_KEY_LEN  2

/*
 * return the next in-order node
 *
 * it updates the stack of nodes that is used to keep
 * track of the parent ones
 */
static void*
next_node(struct tree_info *t, void *node, void **stack, unsigned int *height)
{
	void *next;

	if (tree_node_get_right(t, node)) {

		/* get the leftmost node from right */

		stack[(*height)++] = node;
		next = tree_node_get_right(t, node);

		while (tree_node_get_left(t, next)) {
			stack[(*height)++] = next;
			next = tree_node_get_left(t, next);
		}
	} else {

		/* get the next parent */

		if (*height == 0)
			return NULL;

		next = stack[--*height];

		while (node == tree_node_get_right(t, next)) {
			if (*height == 0)
				return NULL;
			node = next;
			next = stack[--*height];
		}
	}

	return next;
}

static inline const char*
get_balance(struct tree_info *t, void *node)
{
	switch (t->ops->get_balance(node)) {
	case  0: return "  ";
	case  1: return "+ ";
	case -1: return "- ";
	case  2: return "++";
	case -2: return "--";
	}

	return ".."; /* error */
}

/*
 * About the function:
 *
 * output example:
 *   1+
 * 0     3
 *     2   4
 *
 * It print balance factors, however it doesn't
 * print connections between nodes.
 *
 * It reserves a fixed number of characters (node_string_len)
 * per column for every node.
 *
 * Node strings are separed by distance_between_nodes spaces.
 *
 * About parameters:
 *
 * - `array_height` and `array_width` refer to the size of the
 *    character table displayed.
 * - distance_between_nodes: space between two consecutive nodes.
 *   ex.:    365         <- three spaces (assuming balance_len = 0)
 *                 512
 * - balance_len: number of characters reserved to
 *   display the balance sign (see Tip below).
 * - key_len: number of characters to display the key value.
 *
 * Tip while setting `balance_len`: When using a debugger (e.g.
 * GDB) it may be interesting to set `balance_len` to 2. In normal
 * situations it's not necessary because the output will never get
 * unbalanced (e.g. "++" or "--").
 *
 */
int
print_tree(struct tree_info *t, void *root)
{
	/* parameters */
	const unsigned int array_height = PRINT_HEIGHT;
	const unsigned int array_width = PRINT_WIDTH;
	const unsigned int distance_between_nodes = 0;
	const unsigned int balance_len = 2; /* see node_string_len */
	const unsigned int key_len = PRINT_KEY_LEN; /* see node_string_len */

	/* constants depending on parameters */
	const unsigned int array_size = array_height * array_width;
	const unsigned int node_string_len = balance_len + key_len;

	/*
	 * the character table (array), and the string
	 * where key value and balance signal will go
	 */
	char *array = alloca(array_size);
	char *node_string = alloca(node_string_len + /* '\0' */ 1);

	/* stack of nodes is used to traverse the tree */
	void *stack[MAX_STACK_SIZE];

	unsigned int current_height = 0;
	unsigned int current_node = 0;
	void *current = tree_root_get_node(t, root);

	/*
	 * offset in the current line where
	 * the node_string will be inserted
	 */
	unsigned int line_offset;
	int i;

	if (current == NULL)
		return -1;

	/* clean array */
	memset(array, ' ', array_size);

	/* go to the leftmost node (the first one) */
	while (tree_node_get_left(t, current)) {
		stack[current_height++] = current;
		current = tree_node_get_left(t, current);
	}

	/*
	 * by now: `current` is the first node and
	 * we're at line/height `current_height`
	 */

	for (;;) {

		/* line offset where we'll write node_string */
		line_offset = current_node *
		              (node_string_len + distance_between_nodes);

		/* error when we're about to write beyond the boundaries */
		if (current_height > array_height - 1 ||
		    line_offset + node_string_len + 1 > array_width - 1)
			return -1;

		/* put key and balance in node_string */
		snprintf(node_string, node_string_len + 1, "%0*ld%*s",
		         key_len, tree_node_get_key(t, current),
		         balance_len, get_balance(t, current));

		/* write node_string in array */
		for (i = 0; i < node_string_len; i++)
			array[current_height * array_width +
			      (line_offset + i)] = node_string[i];

		/* make current the next node */
		current = next_node(t, current, stack, &current_height);

		if (current == NULL)
			break;

		current_node++;
	}

	/* format and print */

	/* put newlines in array */
	for (i = 0; i < array_height; i++)
		array[i * array_width + (array_width-1)] = '\n';
	array[array_size - 1] = '\0';
	printf("%s\n", array);

	return 0;
}

int
main(int argc, char **argv)
{
	struct list_head tree_list_head;
	struct tree_library *lib;
	struct tree_info tree;
	char input[8];
	void *root;
	void *tmp;

	if (argc < 2) {
		printf("usage: cmd <library>\n");
		return 1;
	}

	lib = tree_library_load(argv[1], &tree_list_head);
	if (!lib) {
		printf("couldn't load library\n");
		return 1;
	}

	tree_info_setup(&tree, &lib->ops);

	root = malloc(tree.root_size);

	printf("insert: i<value>\n"
	       "delete: d<value>\n"
	       "print: p\n"
	       "quit: q or empty\n");

	for (;;) {
		if (read(1, input, sizeof(input)) <= 0)
			break;
		input[sizeof(input) - 1] = '\0';

		switch (input[0]) {
		case 'i':
			/* WARNING: we do not free memory of the elements */
			tmp = malloc(tree.element_size);
			tree_element_set_key(&tree, tmp, atoi(input + 1));
			tree.ops->insert(root, tmp);
			break;
		case 'd':
			tree.ops->delete(root, atoi(input + 1));
			break;
		case 'p':
			printf("%s\n", print_tree(&tree, root)
			       ? "error" : "success");
			break;
		case 'q':
		default:
			goto _go_free_root;
		}
	}

_go_free_root:
	free(root);
_go_unload_trees:
	tree_manager_unload_trees(&tree_list_head);

	return 0;
}
