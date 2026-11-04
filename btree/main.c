#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void run_demo(void) {
    BTree tree;
    btree_init(&tree);

    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17, 1, 3, 8, 9, 15, 18, 25, 40};
    int n = (int)(sizeof(keys) / sizeof(keys[0]));

    printf("Árvore B com t=%d (máx %d chaves/nó)\n", BTREE_T, BTREE_MAX_KEYS);
    printf("Inserindo:");
    for (int i = 0; i < n; i++) {
        printf(" %d", keys[i]);
        btree_insert(&tree, keys[i]);
    }
    printf("\n\nEstrutura:\n");
    btree_print(&tree);

    int busca[] = {6, 15, 99, 1};
    for (int i = 0; i < 4; i++) {
        int idx;
        BNode *found = btree_search(tree.root, busca[i], &idx);
        if (found) {
            printf("busca %d: encontrado (pos %d no nó)\n", busca[i], idx);
        } else {
            printf("busca %d: não encontrado\n", busca[i]);
        }
    }

    btree_free(&tree);
}

static void interactive_menu(void) {
    BTree tree;
    btree_init(&tree);
    char line[128];

    printf("CLI Árvore B (t=%d) — comandos:\n", BTREE_T);
    printf("  i <k>  inserir\n");
    printf("  s <k>  buscar\n");
    printf("  p      imprimir\n");
    printf("  q      sair\n");

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        if (line[0] == 'q') {
            break;
        } else if (line[0] == 'i') {
            int k;
            if (sscanf(line + 1, "%d", &k) == 1) {
                btree_insert(&tree, k);
                printf("inserido %d\n", k);
            }
        } else if (line[0] == 's') {
            int k;
            if (sscanf(line + 1, "%d", &k) == 1) {
                int idx;
                BNode *found = btree_search(tree.root, k, &idx);
                if (found) {
                    printf("encontrado %d (pos %d)\n", k, idx);
                } else {
                    printf("não encontrado\n");
                }
            }
        } else if (line[0] == 'p') {
            btree_print(&tree);
        } else {
            printf("comando desconhecido\n");
        }
    }
    btree_free(&tree);
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "cli") == 0) {
        interactive_menu();
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "help") == 0) {
        printf("Uso: %s [demo|cli]\n", argv[0]);
        return 0;
    }
    run_demo();
    return 0;
}
