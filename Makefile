# Use -ggdb in CFLAGS for debugging

# rpath adds a directory to where runtime linker search for
# libraries. Here it's added the current working directory

LDLIBS = -ldl
LDFLAGS = -Wl,-rpath=.

common_headers += tree_operations.h
common_headers += tree_manager.h
common_headers += single_linked_list.h

# rules generated using `gcc -MM`

.PHONY: all
all: print_tree diff_trees performance_test

# tree manager is used in all programs
tree_manager.o: tree_manager.c $(common_headers)

# Print tree
print_tree.o: print_tree.c $(common_headers)
print_tree: tree_manager.o print_tree.o

# Diff trees
diff_trees.o: diff_trees.c $(common_headers)
diff_trees: tree_manager.o diff_trees.o

# Performance test
performance_test.o: performance_test.c $(common_headers)
performance_test: tree_manager.o performance_test.o
