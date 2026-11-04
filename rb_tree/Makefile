CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
OBJS = node.o rbtree.o main.o

.PHONY: all clean test demo cli

all: rbtree_cli

rbtree_cli: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

node.o: node.c node.h
rbtree.o: rbtree.c rbtree.h node.h
main.o: main.c rbtree.h node.h

test: rbtree_cli
	./rbtree_cli test

demo: rbtree_cli
	./rbtree_cli demo

cli: rbtree_cli
	./rbtree_cli cli

clean:
	rm -f $(OBJS) rbtree_cli
