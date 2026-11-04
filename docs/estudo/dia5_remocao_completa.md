# Dia 5 — Remoção completa + integração

## Objetivos
- RB Tree completa: insert + delete + validate
- CLI de estudos
- Bateria 50 inserções + 30 remoções aleatórias

## Fluxo completo
1. Busca z
2. Se 2 filhos: y = sucessor; senão y = z
3. x = filho de y (ou NIL)
4. `transplant` remove y da árvore (ou promove sucessor)
5. Se y era preto → `deleteFixup(x)`
6. Complexidade: O(log n)

## Prática
```bash
cd rb_tree
make test          # sequência fixa + aleatório 50/30
make cli           # menu interativo
```

Comandos CLI: `i k`, `d k`, `s k`, `p`, `t`, `v`, `q`

## Roteiro de revisão (45 min)
1. Desenhe insert de 7 chaves com cores
2. Remova 3 chaves forçando casos diferentes
3. Explique em voz alta: delete → transplant → fixup
4. Rode `make test` e confirme OK

## Checkpoint
- [ ] `make test` passa
- [ ] Consigo explicar o fluxo sem olhar o código
- [ ] Pronto para a aula prática (Cap. 23)
