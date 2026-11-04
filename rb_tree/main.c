#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void demo_rotations(void) {
    RBTree t;
    rbtree_init(&t);

    /* Monta manualmente 10 — 20 — 30 (como cadeia à direita) e rotaciona. */
    Node *a = create_node(10);
    Node *b = create_node(20);
    Node *c = create_node(30);
    a->color = BLACK;
    b->color = RED;
    c->color = RED;
    t.root = a;
    a->right = b;
    b->parent = a;
    b->right = c;
    c->parent = b;

    printf("Antes de rotateLeft(10):\n");
    rbtree_print_structure(&t);

    rbtree_rotate_left(&t, a);

    printf("Depois de rotateLeft(10):\n");
    rbtree_print_structure(&t);

    rbtree_rotate_right(&t, b);
    printf("Depois de rotateRight(20) — volta:\n");
    rbtree_print_structure(&t);

    rbtree_free(&t);
}

static void run_fixed_sequence(void) {
    RBTree t;
    rbtree_init(&t);
    int keys[] = {10, 20, 30, 15, 25, 5, 1};
    int n = (int)(sizeof(keys) / sizeof(keys[0]));

    printf("Inserindo:");
    for (int i = 0; i < n; i++) {
        printf(" %d", keys[i]);
        rbtree_insert(&t, keys[i]);
        if (!rbtree_check_properties(&t)) {
            printf("\nFALHA após inserir %d\n", keys[i]);
            rbtree_free(&t);
            return;
        }
    }
    printf("\nIn-order: ");
    rbtree_print_inorder(&t);
    printf("Estrutura:\n");
    rbtree_print_structure(&t);
    printf("Propriedades RB: OK\n");

    int del[] = {20, 10, 1, 30};
    for (int i = 0; i < 4; i++) {
        printf("Removendo %d...\n", del[i]);
        rbtree_delete(&t, del[i]);
        if (!rbtree_check_properties(&t)) {
            printf("FALHA após remover %d\n", del[i]);
            rbtree_free(&t);
            return;
        }
        rbtree_print_inorder(&t);
    }
    printf("Propriedades RB após remoções: OK\n");
    rbtree_free(&t);
}

static void run_random_tests(int inserts, int deletes) {
    RBTree t;
    rbtree_init(&t);
    int *keys = (int *)malloc((size_t)inserts * sizeof(int));
    if (!keys) {
        return;
    }

    srand((unsigned)time(NULL));
    for (int i = 0; i < inserts; i++) {
        keys[i] = rand() % 1000;
        rbtree_insert(&t, keys[i]);
        if (!rbtree_check_properties(&t)) {
            fprintf(stderr, "FALHA random insert i=%d key=%d\n", i, keys[i]);
            free(keys);
            rbtree_free(&t);
            exit(1);
        }
    }

    for (int i = 0; i < deletes; i++) {
        int k = keys[rand() % inserts];
        rbtree_delete(&t, k);
        if (!rbtree_check_properties(&t)) {
            fprintf(stderr, "FALHA random delete key=%d\n", k);
            free(keys);
            rbtree_free(&t);
            exit(1);
        }
    }

    printf("Teste aleatório: %d inserções + %d remoções — OK\n", inserts, deletes);
    free(keys);
    rbtree_free(&t);
}

static void interactive_menu(void) {
    RBTree t;
    rbtree_init(&t);
    char line[128];

    printf("CLI Rubro-Negra — comandos:\n");
    printf("  i <k>  inserir\n");
    printf("  d <k>  remover\n");
    printf("  s <k>  buscar\n");
    printf("  p      imprimir in-order\n");
    printf("  t      imprimir árvore\n");
    printf("  v      validar propriedades\n");
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
                rbtree_insert(&t, k);
                printf("inserido %d | válido=%s\n", k,
                       rbtree_check_properties(&t) ? "sim" : "NÃO");
            }
        } else if (line[0] == 'd') {
            int k;
            if (sscanf(line + 1, "%d", &k) == 1) {
                rbtree_delete(&t, k);
                printf("removido %d | válido=%s\n", k,
                       rbtree_check_properties(&t) ? "sim" : "NÃO");
            }
        } else if (line[0] == 's') {
            int k;
            if (sscanf(line + 1, "%d", &k) == 1) {
                Node *n = rbtree_search(&t, k);
                if (n != NIL) {
                    printf("encontrado %d (%s)\n", k, n->color == RED ? "R" : "B");
                } else {
                    printf("não encontrado\n");
                }
            }
        } else if (line[0] == 'p') {
            rbtree_print_inorder(&t);
        } else if (line[0] == 't') {
            rbtree_print_structure(&t);
        } else if (line[0] == 'v') {
            printf("válido=%s\n", rbtree_check_properties(&t) ? "sim" : "NÃO");
        } else {
            printf("comando desconhecido\n");
        }
    }
    rbtree_free(&t);
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "demo") == 0) {
        demo_rotations();
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        run_fixed_sequence();
        run_random_tests(50, 30);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "cli") == 0) {
        interactive_menu();
        return 0;
    }

    printf("Uso: %s [demo|test|cli]\n", argv[0]);
    printf("  demo  — Dia 1: demonstra rotações\n");
    printf("  test  — Dias 2–5: sequência fixa + 50/30 aleatório\n");
    printf("  cli   — menu interativo inserir/remover/validar\n");
    /* default: roda testes */
    run_fixed_sequence();
    run_random_tests(50, 30);
    return 0;
}
