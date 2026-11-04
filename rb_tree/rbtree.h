#ifndef RBTREE_H
#define RBTREE_H

#include "node.h"
#include <stdbool.h>

typedef struct RBTree {
    Node *root;
} RBTree;

void rbtree_init(RBTree *t);
void rbtree_free(RBTree *t);

void rbtree_rotate_left(RBTree *t, Node *x);
void rbtree_rotate_right(RBTree *t, Node *y);

Node *rbtree_search(RBTree *t, int key);
Node *rbtree_minimum(Node *x);
Node *rbtree_maximum(Node *x);

void rbtree_insert(RBTree *t, int key);
void rbtree_delete(RBTree *t, int key);

void rbtree_print_inorder(RBTree *t);
void rbtree_print_structure(RBTree *t);

/* Retorna true se todas as propriedades RB forem satisfeitas. */
bool rbtree_check_properties(RBTree *t);

#endif /* RBTREE_H */
