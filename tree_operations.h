/* 29/01/2018 */

#ifndef TREE_OPERATIONS_H
#define TREE_OPERATIONS_H

struct tree_operations {
	/* sizes */
	size_t (*get_root_size)(void);
	size_t (*get_node_size)(void);

	/* offsets */
	size_t (*get_root_node_offset)(void);
	size_t (*get_left_offset)(void);
	size_t (*get_right_offset)(void);
	size_t (*get_node_offset_in_element)(void);
	size_t (*get_key_offset_in_element)(void);

	/* get balance operation */
	unsigned int (*get_balance)(void *element);

	/* main operations */
	void (*delete)(void *root, unsigned long key);
	void (*insert)(void *root, void *pos);
	void (*init)(void *root);
};

#endif /* TREE_OPERATIONS_H */
