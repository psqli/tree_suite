===============
Tree interfaces
===============

:Date: 2018-03-04

This directory has interfaces to other trees.

Every implementation has the tree type (AVL tree, red-black
tree, ...) and a short name in parentheses as the section
title and provides the following details:

* The implementation nature:

1. iterative or recursive
2. has parent pointer or not
3. store balance factor or height
4. intrusive or not

* A list of author(s), files, the repository (or website), and
  how to use.


AVL tree (ebiggers)
===================

1. iterative
2. has parent pointer
3. store balance factor
4. intrusive

Author(s): Eric Biggers

Files: ebiggers_avl.c

https://github.com/ebiggers/avl_tree

How to use:

1. Download or clone the repository

* Download: <https://github.com/ebiggers/avl_tree/archive/master.zip>
* Clone::
	$ git clone https://github.com/ebiggers/avl_tree.git

2. Rename the directory to "ebiggers_avl"

3. Run ``$ make ebiggers``


AVL tree (pasquali)
===================

1. iterative
2. no parent pointer
3. store balance factor
4. intrusive

Author(s): Ricardo Biehl Pasquali

Files: pasquali_avl.c

How to use:

1. Download or clone the repository

* Download: <https://github.com/ricardobiehl/avl_tree/archive/master.zip>
* Clone::
	$ git clone https://github.com/ricardobiehl/avl_tree.git

2. Rename the directory to "pasquali_avl"

3. Run ``$ make pasquali``
