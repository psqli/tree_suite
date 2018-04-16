/*
 * tree operations
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
 * 29/01/2018: created the already obsolete header that had
 *   the function prototype to register tree operations;
 * 05/02/2018
 *
 * iterative, no parent, uses balance factor, intrusive
 *
 * Interface to Ricardo Pasquali's AVL implementation (see
 * README). <https://github.com/ricardobiehl/avl_tree>
 */

#include <stddef.h> /* offsetof() */

#include "example.h"
#include "avl_tree.h"

/* NOTE: dlsym() does not get 'static' things */

char *magic_string = "binary_tree_module";

size_t
get_root_size(void)
{
	return sizeof(struct avl_root);
}

size_t
get_node_size(void)
{
	return sizeof(struct example);
}

size_t
get_root_node_offset(void)
{
	return offsetof(struct avl_root, avl_node);
}

size_t
get_left_offset(void)
{
	return offsetof(struct avl_node, left);
}

size_t
get_right_offset(void)
{
	return offsetof(struct avl_node, right);
}

size_t
get_node_offset_in_element(void)
{
	return offsetof(struct example, node);
}

size_t
get_key_offset_in_element(void)
{
	return offsetof(struct example, key);
}

unsigned int
get_balance(void *node)
{
	return ((struct avl_node*) node)->balance;
}

void
insert(void *root, void *pos)
{
	struct example *new = pos;

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
	struct avl_root *root = _root;

	*root = AVL_ROOT;
}
