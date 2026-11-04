# Estudo EDA — Árvores Rubro-Negra e Árvore B

Material de recuperação (Caps. 19–24) em C: teoria dia a dia + implementações.

## Cronograma

| Dia | Data | Guia | Código |
|-----|------|------|--------|
| 1 | 04/11 | [dia1_fundamentos.md](docs/estudo/dia1_fundamentos.md) | `rb_tree` rotações |
| 2 | 05/11 | [dia2_insercao.md](docs/estudo/dia2_insercao.md) | insert + fixup |
| 3 | 06/11 | [dia3_prep_remocao.md](docs/estudo/dia3_prep_remocao.md) | transplant / delete BST |
| 4 | 07/11 | [dia4_remocao_parte1.md](docs/estudo/dia4_remocao_parte1.md) | deleteFixup |
| 5 | 08/11 | [dia5_remocao_completa.md](docs/estudo/dia5_remocao_completa.md) | CLI + testes |
| 6 | 09/11 | [dia6_aula_pratica.md](docs/estudo/dia6_aula_pratica.md) | Cap. 23 |
| 7 | 10/11 | [dia7_btree.md](docs/estudo/dia7_btree.md) | `btree` search/insert |
| 8 | 11/11 | [dia8_cap24.md](docs/estudo/dia8_cap24.md) | Cap. 24 |

## Rubro-Negra

```bash
cd rb_tree
make          # compila rbtree_cli
make demo     # Dia 1: rotações
make test     # Dias 2–5: sequência fixa + 50 insert / 30 delete
make cli      # menu interativo
```

## Árvore B

```bash
cd btree
make
make test     # demo de inserção + busca
make cli
```

Grau mínimo padrão: `t=3` (veja `BTREE_T` em `btree.h`).

## Visualizadores

- [Red-Black Tree](https://www.cs.usfca.edu/~galles/visualization/RedBlack.html)
- [B-Tree](https://www.cs.usfca.edu/~galles/visualization/BTree.html)
