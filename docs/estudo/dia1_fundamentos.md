# Dia 1 — Fundamentos + Definição Rubro-Negra (Cap. 19)

## Objetivos
- Revisar BST e rotações
- Entender árvore graduada (rank)
- Dominar as 5 propriedades da Rubro-Negra e black-height

## Teoria

### BST (revisão rápida)
- Filho esquerdo < nó < filho direito
- Busca / inserção: O(h); pior caso h = n (degenerada)

### Rotações
- **rotateLeft(x):** y = x.right sobe; subárvore esquerda de y vira direita de x
- **rotateRight(y):** x = y.left sobe; subárvore direita de x vira esquerda de y
- Rotações preservam a propriedade de BST e mudam a forma (altura local)

### Árvore graduada (rank)
- Cada nó tem um *rank* (grau) relacionado à altura negra / profundidade de balanceamento
- Ideia: restringir diferença de ranks entre pai e filho → árvore balanceada
- Rubro-Negra é um caso particular (códigos de cor = restrições de rank)

### 5 propriedades da Rubro-Negra
1. Todo nó é **vermelho** ou **preto**
2. A **raiz** é preta
3. Toda folha (NIL) é preta
4. Se um nó é vermelho, ambos os filhos são pretos (não há dois vermelhos seguidos)
5. Para todo nó, todo caminho até uma folha NIL tem o **mesmo número de nós pretos** (black-height)

### Black-height bh(x)
- Número de nós pretos em qualquer caminho de x (excluindo x) até uma folha
- Propriedade 5 ⇒ bh é bem definida
- Altura da árvore ≤ 2 · lg(n+1) ⇒ operações O(log n)

## Prática (código)
Arquivos: `rb_tree/node.c`, `rb_tree/rbtree.c` (rotações + impressão)

```bash
cd rb_tree && make && ./rbtree_cli
# opção: demo rotações / imprimir
```

## Exercícios de papel
1. Desenhe uma BST com chaves 7, 3, 11, 1, 5. Faça rotateLeft em 7. A BST se mantém?
2. Marque cores em uma árvore de 5 nós de modo a satisfazer as 5 propriedades.
3. Calcule bh(raiz) e bh de cada filho na sua árvore.
4. Mostre um contraexemplo: dois vermelhos seguidos — qual propriedade quebra?
5. Explique em uma frase por que a propriedade 5 limita a altura.

## Checkpoint
- [ ] Explico as 5 propriedades sem olhar
- [ ] Rotaciono uma árvore no papel e no código
- [ ] Sei o que é black-height
