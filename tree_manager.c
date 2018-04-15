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
 *
 * Read the README
 */

#include <stdlib.h> /* malloc free */
#include <string.h> /* strcmp strrchr strncmp strdup */
#include <sys/types.h>
#include <dirent.h> /* opendir readdir */

/* NOTE: link with -ldl */
#include <dlfcn.h> /* dlopen */

#include "tree_manager.h"

/*
 * Tree library
 * ============
 *
 * Helpers around the dynamic load of the trees.
 *
 * The exposed functions are:
 * - tree_library_load()
 * - tree_manager_unload_trees()
 * - tree_manager_load_trees()
 */

static const char*
get_filename_ext(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	/* dot equals filename when it's a hidden file :-) */
	if(!dot || dot == filename)
		return "";
	return dot + 1;
}

static int
get_symbols(void *library, struct tree_operations *ops)
{

#define IF_NULL_RETURN_ERROR(ptr) \
do { \
	if ((ptr) == NULL) \
		return -1; \
} while (0)

#define __get_symbol(library, operations, symbol) \
  IF_NULL_RETURN_ERROR( (operations)->symbol = dlsym(library, #symbol) )

	/* size */
	__get_symbol(library, ops, get_root_size);
	__get_symbol(library, ops, get_node_size);

	/* offset */
	__get_symbol(library, ops, get_root_node_offset);
	__get_symbol(library, ops, get_left_offset);
	__get_symbol(library, ops, get_right_offset);
	__get_symbol(library, ops, get_node_offset_in_element);
	__get_symbol(library, ops, get_key_offset_in_element);

	__get_symbol(library, ops, get_balance);

	/* tree ops */
	__get_symbol(library, ops, delete);
	__get_symbol(library, ops, insert);
	__get_symbol(library, ops, init);
}

/*
 * 1. Load the library using dlopen()
 * 2. Check if it's a valid tree library by checking the
 *    magic_string symbol
 * 3. Allocate memory for tree_library structure
 * 4. Get the library symbols (functions) and store them in
 *    tree operations structure inside tree_library
 * 5. Insert tree_library in a single linked list of loaded
 *    tree libraries
 *
 * assume filename is not NULL
 */
struct tree_library*
tree_library_load(const char *filename, struct list_head *tree_list)
{
	void *library;
	char **magic_string;
	struct tree_library *new;

	/* rpath is set to . (see Makefile) */
	library = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
	if (library == NULL)
		return NULL;

#define EXPECTED_MAGIC_STRING  "binary_tree_module"

	/* error if magic string isn't found or if it's wrong */
	magic_string = dlsym(library, "magic_string");
	if (magic_string == NULL)
		goto _go_close_library;
	if (strncmp(*magic_string, EXPECTED_MAGIC_STRING,
	            sizeof(EXPECTED_MAGIC_STRING)) != 0)
		goto _go_close_library;

	/* allocate memory for the new tree library */
	new = malloc(sizeof(*new));

	/* get the symbols (functions) from library */
	if (get_symbols(library, &new->ops) == -1)
		goto _go_free_tree_library;

	/* set library's name to filename */
	new->name = strdup(filename);

	/* store library handle so we can close it later */
	new->library = library;

	/* insert library in the linked list */
	list_add(&new->list_node, tree_list);

	return new;

//_go_free_name:
//	free(new->name);
_go_free_tree_library:
	free(new);
_go_close_library:
	dlclose(library);

	return NULL;
}

void
tree_manager_unload_trees(struct list_head *tree_list)
{
	struct list_node *current;
	struct list_node *next;

	list_for_each_safe (current, next, tree_list->first) {
		struct tree_library *tmp;

		tmp = container_of(current, struct tree_library, list_node);

		dlclose(tmp->library);
		free(tmp->name);
		free(tmp);
	}
}

/*
 * dynamic load all trees in current working directory
 *
 * It simply calls tree_library_load() for each .so file.
 *     so = shared object, because it may be shared between
 *     multiple programs :-)
 */
int
tree_manager_load_trees(struct list_head *tree_list)
{
	DIR *directory;
	struct dirent *current_file;
	char *current_filename;

	/* open the current working directory */
	directory = opendir(".");
	if (directory == NULL)
		return -1;

	/* for each file in directory */
	while (current_file = readdir(directory)) {
		current_filename = current_file->d_name;

		/* skip current and parent entries */
		if (strcmp(current_filename, ".") == 0
		    || strcmp(current_filename, "..") == 0)
			continue;

		/* skip if file extension is not .so */
		if (strcmp(get_filename_ext(current_filename), "so") != 0)
			continue;

		/*
		 * Dynamic load the shared library.
		 * If it's a valid tree library, set up a
		 * 'struct tree_library'
		 */
		tree_library_load(current_filename, tree_list);
	}

	closedir(directory);

	return 0;
}

/*
 * Tree information
 * ================
 */

void
tree_info_setup(struct tree_info *info, struct tree_operations *ops)
{
	/* get sizes */
	info->root_size = ops->get_root_size();
	info->element_size = ops->get_node_size();

	/* get offsets */
	info->root_node_offset = ops->get_root_node_offset();
	info->left_child_offset = ops->get_left_offset();
	info->right_child_offset = ops->get_right_offset();
	info->node_offset_in_element = ops->get_node_offset_in_element();
	info->key_offset_in_element = ops->get_key_offset_in_element();

	/* include a pointer to tree operations inside tree_info */
	info->ops = ops;
}

/*
 * Tree memory
 * ===========
 */

void
tree_memory_free(struct tree_memory *m)
{
	free(m->addr);
}

void
tree_memory_allocate(struct tree_memory *m, struct tree_info *i,
                     unsigned int size)
{
	/* allocate memory to root pointer and tree elements */
	m->addr = malloc(i->root_size + i->element_size * size);
	m->root = m->addr;
	m->array = m->addr + i->root_size;
}

void
tree_fill_in_order(struct tree_memory *m, struct tree_info *i,
                   unsigned long current)
{
	unsigned long *tmp;

	/* set array in-order */
	while (current--) {
		tmp = m->array + current * i->element_size +
		      i->key_offset_in_element;
		*tmp = current;
	}
}

void
tree_randomize(struct tree_memory *m, struct tree_info *i,
               unsigned long current)
{
	unsigned long *a, *b;
	unsigned long elements = current;
	unsigned long tmp, random_idx;

	/* shuffle array */
	while (current--) {
		/* exchange values between 'a' and 'b' */

		/* define the pointers */
		a = m->array + current * i->element_size +
		    i->key_offset_in_element;
		random_idx = random() % elements;
		b = m->array + random_idx * i->element_size +
		    i->key_offset_in_element;

		/* exchange values */
		tmp = *a;
		*a = *b;
		*b = tmp;
	}
}

void
tree_copy_keys(struct tree_memory *dst_mem,
               struct tree_info *dst_info,
               struct tree_memory *src_mem,
               struct tree_info *src_info,
               unsigned long current)
{
	unsigned long *dst, *src;

	while (current--) {
		/* define the pointers */
		dst = dst_mem->array + current * dst_info->element_size +
		      dst_info->key_offset_in_element;
		src = src_mem->array + current * src_info->element_size +
		      src_info->key_offset_in_element;

		*dst = *src;
	}
}

void
tree_assign_keys(struct tree_memory *m, struct tree_info *i,
                 unsigned long *key_array, unsigned long current)
{
	unsigned long *dst;

	while (current--) {
		/* define the pointer */
		dst = m->array + current * i->element_size +
		      i->key_offset_in_element;

		*dst = key_array[current];
	}
}
