CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
OBJS = btree.o main.o

.PHONY: all clean test cli

all: btree_cli

btree_cli: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

btree.o: btree.c btree.h
main.o: main.c btree.h

test: btree_cli
	./btree_cli

cli: btree_cli
	./btree_cli cli

clean:
	rm -f $(OBJS) btree_cli
