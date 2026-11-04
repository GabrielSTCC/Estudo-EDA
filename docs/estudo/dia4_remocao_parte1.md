# Dia 4 — Remoção Rubro-Negra (parte 1) (Cap. 22)

## Objetivos
- Memorizar os 4 casos de `deleteFixup` (lado esquerdo)
- Saber qual rotação/recoloração cada caso usa

## Teoria — `deleteFixup` (x é o “duplo-preto”, w = irmão)

Enquanto x não é raiz e x é preto:

### Caso 1 — irmão w vermelho
- w → preto; pai → vermelho; `rotateLeft(pai)`; atualiza w
- Objetivo: reduzir aos casos 2–4 (irmão preto)

### Caso 2 — w preto, ambos sobrinhos pretos
- w → vermelho; sobe x para o pai (a “dívida” sobe)
- Se o pai era vermelho, no fim do loop fica preto e resolve

### Caso 3 — w preto, sobrinho **interno** vermelho, **externo** preto
- (x filho esquerdo ⇒ interno = w.left)
- Recolorir: w.left → preto; w → vermelho; `rotateRight(w)`; atualiza w
- Cai no caso 4

### Caso 4 — w preto, sobrinho **externo** vermelho
- (x filho esquerdo ⇒ externo = w.right)
- w.color = pai.color; pai → preto; w.right → preto; `rotateLeft(pai)`
- x = raiz (termina)

Simetria: se x é filho direito, espelhe left↔right e rotateLeft↔rotateRight.

## Prática
Código: `delete_fixup` em `rb_tree/rbtree.c` (lado esquerdo e direito).

Monte árvores pequenas e force cada caso (use o visualizador + seu CLI).

```bash
cd rb_tree && make && ./rbtree_cli cli
```

## Checklist dos 4 casos
| Caso | Irmão | Sobrinhos | Ação-chave |
|------|-------|-----------|------------|
| 1 | vermelho | — | rotaciona pai, recolore |
| 2 | preto | ambos pretos | w vermelho, sobe |
| 3 | preto | interno vermelho | rotaciona w |
| 4 | preto | externo vermelho | rotaciona pai, fim |

## Checkpoint
- [ ] Listo os 4 casos e a ação de cada um
- [ ] Implementação do lado esquerdo compreendida linha a linha
