# Dia 2 — Inserção Rubro-Negra (Cap. 20)

## Objetivos
- Dominar inserção BST + nó vermelho
- Conhecer os 3 casos do `insertFixup` (e o simétrico)
- Validar a árvore após cada inserção

## Teoria

### Passos
1. Inserir como em BST
2. Colorir o novo nó de **vermelho**
3. Corrigir violações com `insertFixup`
4. Garantir raiz preta no final

### Casos (z = nó atual; pai vermelho)

| Caso | Condição | Ação |
|------|----------|------|
| 1 | Tio **vermelho** | Recolorir pai e tio de preto; avô de vermelho; subir z para o avô |
| 2 | Tio preto, z é filho “interno” (zig-zag) | Rotacionar no **pai**; cair no caso 3 |
| 3 | Tio preto, z é filho “externo” (zig-zig) | Pai → preto; avô → vermelho; rotacionar no **avô** |

Sempre existe a versão simétrica (pai é filho direito do avô).

### Por que começa vermelho?
- Não altera black-height (prop. 5)
- Pode violar prop. 4 (dois vermelhos) → fixup cuida disso

## Prática
```bash
cd rb_tree && make test
# ou: ./rbtree_cli cli   → i 10, i 20, i 30, ... t, v
```

Sequência sugerida: `10, 20, 30, 15, 25, 5, 1`

## Exercícios de papel
1. Insira 10, 20, 30. Qual caso ocorre? Desenhe antes/depois.
2. Continue com 15. Quais rotações?
3. Classifique cada passo: caso 1, 2 ou 3.
4. Mostre que após o fixup não há dois vermelhos seguidos.

## Flashcards
- Tio vermelho → ?
- Zig-zag → ?
- Zig-zig → ?
- Cor final da raiz → ?

## Checkpoint
- [ ] Inserir 10 chaves mantendo propriedades válidas
- [ ] Desenhar os 3 casos principais
