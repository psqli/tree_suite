/*
 * performance test for trees
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
 * 14/01/2018
 *
 * 30/01/2018: make this a generic test.
 */

#include <limits.h> /* UINT_MAX */
#include <stdio.h> /* fflush() */
#include <stddef.h>
#include <stdlib.h> /* random() */
#include <string.h> /* memset */
#include <time.h>

#include "tree_manager.h"

/* a million operations is the default */
#define N_OPS  1000000

enum {
	INORDER_TEST,
	RANDOM_TEST,
	TEST_LAST,
};

struct test_result {
	struct timespec elapsed_time[TEST_LAST];
};

/* stop - start */
static void
time_diff(struct timespec *diff, struct timespec *stop, struct timespec *start)
{
	if (stop->tv_nsec < start->tv_nsec) {
		/* here we assume (stop->tv_sec - start->tv_sec) is not zero */
		diff->tv_sec = stop->tv_sec - start->tv_sec - 1;
		diff->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
	} else {
		diff->tv_sec = stop->tv_sec - start->tv_sec;
		diff->tv_nsec = stop->tv_nsec - start->tv_nsec;
	}
}

static void
fill_in_order(unsigned long *array, unsigned long current)
{
	while (current--)
		array[current] = current;
}

/*
 * NOTE: do not use XOR swap because random_idx may be
 * equal current
 */
static void
randomize(unsigned long *array, unsigned long current)
{
	unsigned long elements = current;
	unsigned long tmp_value, random_idx;

	/* shuffle array */
	while (current--) {
		/*
		 * exchange values (swap) between
		 * array[current] and array[random_idx]
		 */

		random_idx = random() % elements;

		tmp_value = array[current];
		array[current] = array[random_idx];
		array[random_idx] = tmp_value;
	}
}

static void
prepare_random_key_array(unsigned long *random_key_array, unsigned long size)
{
	fill_in_order(random_key_array, size);
	randomize(random_key_array, size);
}

/*
 * This is the function where the test happens.
 *
 * random_key_array is initialized in main() and is used
 * to keep the same random keys during multiple tests.
 */
static void
do_test(struct tree_operations *ops, struct test_result *result,
        unsigned long *random_key_array)
{
	struct tree_info tree_info;
	struct tree_memory tree_memory;
	struct timespec start_time, stop_time;
	unsigned long i;

	tree_info_setup(&tree_info, ops);
	tree_memory_allocate(&tree_memory, &tree_info, N_OPS);
	/* write in the memory so it will be in cache */
	memset(tree_memory.addr, 0,
	       tree_info.root_size + tree_info.element_size * N_OPS);
	ops->init(tree_memory.root);

	/*
	 * in-order test
	 */

	tree_fill_in_order(&tree_memory, &tree_info, N_OPS);

	clock_gettime(CLOCK_MONOTONIC, &start_time);

	for (i = 0; i < N_OPS; i++)
		tree_insert(&tree_memory, &tree_info, i);

	for (i = 0; i < N_OPS; i++)
		tree_delete(&tree_memory, &tree_info, i);

	clock_gettime(CLOCK_MONOTONIC, &stop_time);

	/* store 'in-order test' running time in test result */
	time_diff(&result->elapsed_time[INORDER_TEST],
	          &stop_time, &start_time);

	/*
	 * random test
	 */

	/*
	 * we do this way to keep the same
	 * random keys during multiple tests
	 */
	tree_assign_keys(&tree_memory, &tree_info, random_key_array, N_OPS);

	clock_gettime(CLOCK_MONOTONIC, &start_time);

	for (i = 0; i < N_OPS; i++)
		tree_insert(&tree_memory, &tree_info, i);

	for (i = 0; i < N_OPS; i++)
		tree_delete(&tree_memory, &tree_info, i);

	clock_gettime(CLOCK_MONOTONIC, &stop_time);

	/* store 'random test' running time in test result */
	time_diff(&result->elapsed_time[RANDOM_TEST],
	          &stop_time, &start_time);

	tree_memory_free(&tree_memory);
}

int
main(void)
{
	struct test_result result;
	struct timespec time_seed;
	struct list_head tree_list_head = LIST_HEAD_INIT;
	struct list_node *current;

	unsigned long random_key_array[N_OPS];

	tree_manager_load_trees(&tree_list_head);

	/* set random() seed */
	clock_gettime(CLOCK_REALTIME, &time_seed);
	srandom(time_seed.tv_nsec % UINT_MAX);

	prepare_random_key_array(random_key_array, N_OPS);

	list_for_each (current, tree_list_head.first) {
		struct tree_library *tmp;

		tmp = container_of(current, struct tree_library, list_node);

		do_test(&tmp->ops, &result, random_key_array);

		printf("Tree %s\n", tmp->name);
		printf("  in-order: %ld.%09ld\n",
	               result.elapsed_time[INORDER_TEST].tv_sec,
	               result.elapsed_time[INORDER_TEST].tv_nsec);
		printf("  random: %ld.%09ld\n",
	               result.elapsed_time[RANDOM_TEST].tv_sec,
	               result.elapsed_time[RANDOM_TEST].tv_nsec);

		fflush(stdout);
	}

	tree_manager_unload_trees(&tree_list_head);

	return 0;
}
