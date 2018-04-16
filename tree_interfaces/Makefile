# Generate position independent code
CFLAGS = -fpic
# Create a shared library
LDFLAGS = -shared

all: ebiggers \
     pasquali

# AVL tree (ebiggers)

ebiggers_impl_dir = ebiggers_avl

ebiggers_avl_tree: \
  $(ebiggers_impl_dir)/avl_tree.c \
  $(ebiggers_impl_dir)/avl_tree.h
	cd $(ebiggers_impl_dir) && \
	$(CC) $(CFLAGS) -c avl_tree.c
ebiggers_interface: \
  ebiggers_avl.c \
  $(ebiggers_impl_dir)/avl_tree.h
	$(CC) $(CFLAGS) -I $(ebiggers_impl_dir) -c ebiggers_avl.c
ebiggers: ebiggers_avl_tree ebiggers_interface
	$(CC) -o ebiggers_avl_tree.so \
	      $(LDFLAGS) \
	      $(ebiggers_impl_dir)/avl_tree.o \
	      ebiggers_avl.o

# AVL tree (pasquali)

pasquali_impl_dir = pasquali_avl

pasquali_avl_tree: \
  $(pasquali_impl_dir)/avl_tree.c \
  $(pasquali_impl_dir)/avl_tree.h
	cd $(pasquali_impl_dir) && \
	$(CC) $(CFLAGS) -c avl_tree.c
pasquali_example: \
  $(pasquali_impl_dir)/example.c \
  $(pasquali_impl_dir)/avl_tree.h \
  $(pasquali_impl_dir)/example.h
	cd $(pasquali_impl_dir) && \
	$(CC) $(CFLAGS) -c example.c
pasquali_interface: \
  pasquali_avl.c \
  $(pasquali_impl_dir)/avl_tree.h \
  $(pasquali_impl_dir)/example.h
	$(CC) $(CFLAGS) -I $(pasquali_impl_dir) -c pasquali_avl.c
pasquali: pasquali_avl_tree pasquali_example pasquali_interface
	$(CC) -o pasquali_avl_tree.so \
	      $(LDFLAGS) \
	      $(pasquali_impl_dir)/example.o \
	      $(pasquali_impl_dir)/avl_tree.o \
	      pasquali_avl.o
