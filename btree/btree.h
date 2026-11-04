#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>

/*
 * Árvore B de ordem mínima t (CLRS):
 * - Todo nó (exceto raiz) tem pelo menos t-1 chaves
 * - Todo nó tem no máximo 2t-1 chaves
 * - Nó interno com k chaves tem k+1 filhos
 * - Todas as folhas na mesma profundidade
 */

#ifndef BTREE_T
#define BTREE_T 3
#endif

#define BTREE_MAX_KEYS (2 * BTREE_T - 1)
#define BTREE_MAX_CHILDREN (2 * BTREE_T)

typedef struct BNode {
    int n;                              /* número de chaves */
    int keys[BTREE_MAX_KEYS];
    struct BNode *children[BTREE_MAX_CHILDREN];
    bool leaf;
} BNode;

typedef struct BTree {
    BNode *root;
    int t; /* grau mínimo */
} BTree;

void btree_init(BTree *tree);
void btree_free(BTree *tree);

BNode *btree_search(BNode *x, int key, int *index_out);
void btree_insert(BTree *tree, int key);

void btree_print(BTree *tree);

#endif /* BTREE_H */
