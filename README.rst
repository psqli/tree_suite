==========
Tree suite
==========

:Date: 2018-02-23

Allow to write generic code that interact with trees by
implementing them as modules (using shared libraries).

We provide a performance test, a check if trees are equal,
and perhaps some other programs.


The tree manager
================

``tree_manager.c``

Trees are dynamic loaded libraries.


Tree library
------------

``tree_library_load()`` loads the library passed in
*filename*, get its operations (see `Tree operations`_
section) and set up ``struct tree_library``.

The ``struct tree_library`` is inserted in a single linked
list, so all libraries can be freed at once in
``tree_manager_unload_trees()``.

The ``tree_manager_load_trees()`` function simply calls
``tree_library_load()`` for every file with the *.so*
extension in the current working directory.


Tree information
----------------

``struct tree_info`` is set up in ``tree_info_setup()``. It
has variables with the sizes and offsets returned by the
operations.


Tree memory
-----------

``struct tree_memory`` is set up in ``tree_memory_allocate()``,
where it's allocated an array of *size* elements which can be
inserted/deleted in tree.

``tree_fill_in_order()``, ``tree_randomize()``,
``tree_copy_keys()`` and ``tree_assign_keys()`` manipulate
the keys of those elements.


Tree operations
===============

``struct tree_operations``

Main operations:

* delete: Delete element from tree based on its key.
* insert: Insert element in tree.
* init: Initialize tree's root.

Misc operations:

* get_balance: Get balance factor of a node.

Operations that get sizes:

* get_root_size: Get the size of the root structure.
* get_element_size: Get the size of the element structure.

Operations that get offsets:

* get_root_node_offset: Get offset of the root node in root
  structure.
* get_left_offset: Get offset of the left pointer in node
  structure.
* get_right_offset: Get offset of the right pointer in node
  structure.
* get_node_offset_in_element: Get offset of the tree node in
  element structure.
* get_key_offset_in_element: Get offset of the key pointer in
  element structure.
