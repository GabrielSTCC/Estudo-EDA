# Dia 8 — Cap. 24: Árvore B em aula + reforço (11/11)

## Antes / durante a aula
- Leve os desenhos do Dia 7 e o código em `btree/`
- Confira a definição de “ordem” do professor (às vezes ordem = máx. filhos = 2t)
- Anote se a disciplina usa inserção top-down ou bottom-up

## Depois da aula (~1 h)

### Fechar insert
```bash
cd btree && make test
```
Confirme splits na saída impressa.

### 5 exercícios de papel
1. Defina t=3. Qual o mínimo/máximo de chaves em um nó interno não-raiz?
2. Nó cheio com chaves `[10,20,30,40,50]` (t=3). Após split, o que sobe e como ficam os filhos?
3. Desenhe a árvore após inserir: 50, 40, 30, 20, 10 (t=2).
4. Escreva o pseudocódigo de `search` em 5 linhas.
5. Compare: por que B-Tree é preferível a RB em índice de disco?

### Respostas-guia (confira depois de tentar)
1. Mín t−1=2; máx 2t−1=5.
2. Sobe 30; esq `[10,20]`; dir `[40,50]`.
3. (faça no papel — use o visualizador para conferir)
4. Enquanto não achar: avance índice; se folha falha; senão desça children[i].
5. Menos níveis ⇒ menos leituras de bloco.

## Checkpoint final do plano
- [ ] Explico as 5 propriedades RB e black-height
- [ ] Implemento insert + delete RB em C com validação (`rb_tree: make test`)
- [ ] Desenho casos de inserção e os 4 de remoção
- [ ] Explico ordem t, split e busca em Árvore B
- [ ] Esboço C de B-Tree com search + insert (`btree: make test`)
