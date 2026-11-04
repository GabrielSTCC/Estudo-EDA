#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

static BNode *create_node(bool leaf) {
    BNode *node = (BNode *)calloc(1, sizeof(BNode));
    if (!node) {
        return NULL;
    }
    node->leaf = leaf;
    node->n = 0;
    return node;
}

void btree_init(BTree *tree) {
    tree->t = BTREE_T;
    tree->root = create_node(true);
}

static void free_node(BNode *x) {
    if (!x) {
        return;
    }
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            free_node(x->children[i]);
        }
    }
    free(x);
}

void btree_free(BTree *tree) {
    free_node(tree->root);
    tree->root = NULL;
}

BNode *btree_search(BNode *x, int key, int *index_out) {
    int i = 0;
    while (i < x->n && key > x->keys[i]) {
        i++;
    }
    if (i < x->n && key == x->keys[i]) {
        if (index_out) {
            *index_out = i;
        }
        return x;
    }
    if (x->leaf) {
        return NULL;
    }
    return btree_search(x->children[i], key, index_out);
}

/* Parte o filho cheio y = x->children[i] (y tem 2t-1 chaves). */
static void split_child(BTree *tree, BNode *x, int i) {
    int t = tree->t;
    BNode *y = x->children[i];
    BNode *z = create_node(y->leaf);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        z->keys[j] = y->keys[j + t];
    }
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children[j] = y->children[j + t];
        }
    }
    y->n = t - 1;

    for (int j = x->n; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z;

    for (int j = x->n - 1; j >= i; j--) {
        x->keys[j + 1] = x->keys[j];
    }
    x->keys[i] = y->keys[t - 1];
    x->n += 1;
}

static void insert_non_full(BTree *tree, BNode *x, int key) {
    int i = x->n - 1;
    if (x->leaf) {
        while (i >= 0 && key < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        if (i >= 0 && x->keys[i] == key) {
            return; /* duplicata */
        }
        x->keys[i + 1] = key;
        x->n += 1;
    } else {
        while (i >= 0 && key < x->keys[i]) {
            i--;
        }
        if (i >= 0 && x->keys[i] == key) {
            return;
        }
        i++;
        if (x->children[i]->n == 2 * tree->t - 1) {
            split_child(tree, x, i);
            if (key > x->keys[i]) {
                i++;
            } else if (key == x->keys[i]) {
                return;
            }
        }
        insert_non_full(tree, x->children[i], key);
    }
}

void btree_insert(BTree *tree, int key) {
    BNode *r = tree->root;
    if (r->n == 2 * tree->t - 1) {
        BNode *s = create_node(false);
        tree->root = s;
        s->children[0] = r;
        split_child(tree, s, 0);
        insert_non_full(tree, s, key);
    } else {
        insert_non_full(tree, r, key);
    }
}

static void print_node(BNode *x, int depth) {
    if (!x) {
        return;
    }
    for (int d = 0; d < depth; d++) {
        printf("  ");
    }
    printf("[");
    for (int i = 0; i < x->n; i++) {
        if (i) {
            printf(" ");
        }
        printf("%d", x->keys[i]);
    }
    printf("]%s\n", x->leaf ? " (folha)" : "");
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            print_node(x->children[i], depth + 1);
        }
    }
}

void btree_print(BTree *tree) {
    if (!tree->root || tree->root->n == 0) {
        printf("(vazia)\n");
        return;
    }
    print_node(tree->root, 0);
}
