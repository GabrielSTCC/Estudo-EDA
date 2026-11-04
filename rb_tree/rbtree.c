#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

static void free_subtree(Node *x) {
    if (x == NIL) {
        return;
    }
    free_subtree(x->left);
    free_subtree(x->right);
    free_node(x);
}

void rbtree_init(RBTree *t) {
    t->root = NIL;
}

void rbtree_free(RBTree *t) {
    free_subtree(t->root);
    t->root = NIL;
}

void rbtree_rotate_left(RBTree *t, Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL) {
        t->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rbtree_rotate_right(RBTree *t, Node *y) {
    Node *x = y->left;
    y->left = x->right;
    if (x->right != NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NIL) {
        t->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

Node *rbtree_search(RBTree *t, int key) {
    Node *x = t->root;
    while (x != NIL && key != x->key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return x;
}

Node *rbtree_minimum(Node *x) {
    while (x->left != NIL) {
        x = x->left;
    }
    return x;
}

Node *rbtree_maximum(Node *x) {
    while (x->right != NIL) {
        x = x->right;
    }
    return x;
}

static void insert_fixup(RBTree *t, Node *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            Node *y = z->parent->parent->right; /* tio */
            if (y->color == RED) {
                /* Caso 1: tio vermelho → recolorir */
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    /* Caso 2: zig-zag → rotacionar no pai */
                    z = z->parent;
                    rbtree_rotate_left(t, z);
                }
                /* Caso 3: zig-zig → rotacionar no avô + recolorir */
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_rotate_right(t, z->parent->parent);
            }
        } else {
            Node *y = z->parent->parent->left; /* tio (simétrico) */
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rbtree_rotate_right(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_rotate_left(t, z->parent->parent);
            }
        }
    }
    t->root->color = BLACK;
}

void rbtree_insert(RBTree *t, int key) {
    Node *z = create_node(key);
    if (!z) {
        fprintf(stderr, "falha ao alocar nó\n");
        return;
    }

    Node *y = NIL;
    Node *x = t->root;
    while (x != NIL) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else if (z->key > x->key) {
            x = x->right;
        } else {
            /* chave duplicada: ignora */
            free_node(z);
            return;
        }
    }

    z->parent = y;
    if (y == NIL) {
        t->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = NIL;
    z->right = NIL;
    z->color = RED;
    insert_fixup(t, z);
}

static void transplant(RBTree *t, Node *u, Node *v) {
    if (u->parent == NIL) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

static void delete_fixup(RBTree *t, Node *x) {
    while (x != t->root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node *w = x->parent->right; /* irmão */
            if (w->color == RED) {
                /* Caso 1: irmão vermelho */
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_rotate_left(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                /* Caso 2: irmão preto, sobrinhos pretos */
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    /* Caso 3: sobrinho interno vermelho, externo preto */
                    w->left->color = BLACK;
                    w->color = RED;
                    rbtree_rotate_right(t, w);
                    w = x->parent->right;
                }
                /* Caso 4: sobrinho externo vermelho */
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rbtree_rotate_left(t, x->parent);
                x = t->root;
            }
        } else {
            Node *w = x->parent->left; /* irmão (simétrico) */
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rbtree_rotate_right(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rbtree_rotate_left(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rbtree_rotate_right(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

void rbtree_delete(RBTree *t, int key) {
    Node *z = rbtree_search(t, key);
    if (z == NIL) {
        return;
    }

    Node *y = z;
    Color y_original_color = y->color;
    Node *x;

    if (z->left == NIL) {
        x = z->right;
        transplant(t, z, z->right);
    } else if (z->right == NIL) {
        x = z->left;
        transplant(t, z, z->left);
    } else {
        y = rbtree_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free_node(z);

    if (y_original_color == BLACK) {
        delete_fixup(t, x);
    }
}

static void print_inorder_rec(Node *x) {
    if (x == NIL) {
        return;
    }
    print_inorder_rec(x->left);
    printf("%d(%s) ", x->key, x->color == RED ? "R" : "B");
    print_inorder_rec(x->right);
}

void rbtree_print_inorder(RBTree *t) {
    if (t->root == NIL) {
        printf("(vazia)\n");
        return;
    }
    print_inorder_rec(t->root);
    printf("\n");
}

static void print_structure_rec(Node *x, int depth, char side) {
    if (x == NIL) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%c-%d[%s]\n", side, x->key, x->color == RED ? "R" : "B");
    print_structure_rec(x->left, depth + 1, 'L');
    print_structure_rec(x->right, depth + 1, 'R');
}

void rbtree_print_structure(RBTree *t) {
    if (t->root == NIL) {
        printf("(vazia)\n");
        return;
    }
    print_structure_rec(t->root, 0, '*');
}

/* ---- Validação das propriedades RB ---- */

static bool check_no_red_red(Node *x) {
    if (x == NIL) {
        return true;
    }
    if (x->color == RED) {
        if (x->left->color == RED || x->right->color == RED) {
            return false;
        }
    }
    return check_no_red_red(x->left) && check_no_red_red(x->right);
}

/* Retorna black-height ou -1 se inconsistente. */
static int black_height(Node *x) {
    if (x == NIL) {
        return 0; /* NIL conta como preto, mas bh do caminho a partir do pai
                     já contabiliza via recursão abaixo */
    }
    int left = black_height(x->left);
    int right = black_height(x->right);
    if (left < 0 || right < 0 || left != right) {
        return -1;
    }
    return left + (x->color == BLACK ? 1 : 0);
}

bool rbtree_check_properties(RBTree *t) {
    if (t->root == NIL) {
        return true;
    }
    /* Prop. 2: raiz preta */
    if (t->root->color != BLACK) {
        fprintf(stderr, "RB check: raiz não é preta\n");
        return false;
    }
    /* Prop. 4: sem dois vermelhos seguidos */
    if (!check_no_red_red(t->root)) {
        fprintf(stderr, "RB check: dois vermelhos consecutivos\n");
        return false;
    }
    /* Prop. 5: black-height uniforme
       (Prop. 1 e 3 garantidas pela representação: Color + NIL preto) */
    if (black_height(t->root) < 0) {
        fprintf(stderr, "RB check: black-height inconsistente\n");
        return false;
    }
    return true;
}
