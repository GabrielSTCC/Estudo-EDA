#include "node.h"
#include <stdlib.h>

Node NIL_NODE = {0, BLACK, NULL, NULL, NULL};

Node *create_node(int key) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        return NULL;
    }
    n->key = key;
    n->color = RED; /* novos nós começam vermelhos (inserção RB) */
    n->left = NIL;
    n->right = NIL;
    n->parent = NIL;
    return n;
}

void free_node(Node *node) {
    if (node && node != NIL) {
        free(node);
    }
}
