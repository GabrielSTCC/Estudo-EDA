#ifndef NODE_H
#define NODE_H

typedef enum { RED = 0, BLACK = 1 } Color;

typedef struct Node {
    int key;
    Color color;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
} Node;

/* Sentinela NIL: todos os ponteiros "nulos" apontam para ela (preta). */
extern Node NIL_NODE;
#define NIL (&NIL_NODE)

Node *create_node(int key);
void free_node(Node *node);

#endif /* NODE_H */
