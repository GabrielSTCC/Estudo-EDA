# Dia 7 — Árvore B: teoria e esboço (prep. Cap. 24)

## Objetivos
- Entender motivação, ordem `t` e propriedades
- Busca e split; esboço de inserção em C
- Diferenciar RB Tree vs B-Tree

## Teoria

### Motivação
- Otimizar I/O em disco: muitos filhos por nó ⇒ árvores baixas
- Índices de banco de dados / sistemas de arquivos

### Ordem / grau mínimo `t` (CLRS)
- Todo nó (exceto raiz) tem **≥ t−1** chaves
- Todo nó tem **≤ 2t−1** chaves
- Nó interno com k chaves tem **k+1** filhos
- Raiz tem ≥ 1 chave (se não vazia)
- Todas as **folhas na mesma profundidade**

Ex.: t=3 ⇒ entre 2 e 5 chaves por nó (não-raiz); até 6 filhos.

### Busca
- Dentro do nó: percorre chaves ordenadas
- Desce ao filho adequado (como BST multiway)

### Split
- Nó cheio (2t−1 chaves): chave do meio sobe para o pai
- Esquerda fica com t−1 chaves; direita com t−1
- Inserção tipicamente **top-down**: parte filhos cheios no caminho

### Remoção (visão geral)
- Borrow do irmão ou merge se irmão mínimo
- Mais trabalhosa; priorize definição + inserção se o tempo apertar

### RB vs B-Tree
| | Rubro-Negra | Árvore B |
|--|-------------|----------|
| Filhos/nó | 2 | até 2t |
| Balanceamento | cores + rotações | splits/merges |
| Uso típico | memória | disco / índices |

## Prática
```bash
cd btree && make test
make cli
```

Structs: `BNode` com `keys[]`, `children[]`, `n`, `leaf`.

## Exercícios de papel (t=2 ou t=3)
1. Insira 1..10 em B-Tree t=2. Marque cada split.
2. Busque a chave 7: quantos nós visitados?
3. Qual a altura aproximada vs uma BST degenerada com as mesmas chaves?

## Checkpoint
- [ ] Explico ordem t e propriedades
- [ ] Desenho inserção com split
- [ ] `btree_search` + `btree_insert` compilam e rodam
