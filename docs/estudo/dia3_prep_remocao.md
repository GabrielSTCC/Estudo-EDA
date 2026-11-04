# Dia 3 — Reforço inserção + preparação remoção

## Objetivos
- Fechar confiança na inserção (testes aleatórios + extremos)
- Implementar peças BST da remoção: `search`, `minimum`, `transplant`, delete estrutural
- Entender o “nó duplo-preto”

## Teoria

### Remoção BST (3 casos)
1. **0 filhos:** só desliga o nó
2. **1 filho:** substitui pelo filho (`transplant`)
3. **2 filhos:** substitui pela chave do sucessor (mínimo da subárvore direita) e remove o sucessor

### `transplant(u, v)`
- Faz o pai de `u` apontar para `v` no lugar de `u`
- Atualiza `v.parent`

### Por que remoção RB é difícil?
- Remover um nó **preto** reduz black-height em um caminho → viola prop. 5
- Ideia: o “buraco” é tratado como **duplo-preto** (x carrega uma “dívida” de preto)
- `deleteFixup` sobe ou rotaciona/recolore até a dívida sumir

### Remoção “fácil”
- Remover folha **vermelha**: não muda black-height → sem fixup
- Se o nó removido (ou sucessor) era preto → chama `deleteFixup`

## Prática
```bash
cd rb_tree && make test
./rbtree_cli cli
# inserir vários, remover folhas vermelhas e nós pretos, validar com v
```

Funções já no código: `rbtree_search`, `rbtree_minimum`, `transplant` (estática), `rbtree_delete`.

## Exercícios de papel
1. Remova uma folha vermelha: a árvore continua válida sem fixup?
2. Remova um nó preto com um filho vermelho: o que acontece com a cor do filho após o fixup?
3. Árvore com 2 filhos no nó a remover: marque o sucessor e diga se o fixup usa a cor do sucessor.

## Checkpoint
- [ ] Delete estrutural OK (0/1/2 filhos)
- [ ] Explico o que é duplo-preto em uma frase
