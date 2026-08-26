# Tabela Hash em C++

Implementação de uma tabela hash com **encadeamento separado** e quatro funções de
espalhamento selecionáveis em tempo de execução: **divisibilidade**, **multiplicidade**,
**dobra** e **análise dos dígitos**.

## Estrutura

```
tabela-hash/
├── include/tabela_hash.hpp        Interface da tabela
├── src/tabela_hash.cpp            Implementação dos quatro métodos e das operações
├── src/main.cpp                   Menu interativo e modo demonstração
├── tests/teste_tabela_hash.cpp    Suíte de testes (52 verificações)
└── Makefile
```

## Como compilar e executar

```bash
cd tabela-hash

make            # compila o programa e os testes em bin/
make executar   # abre o menu interativo
make demo       # roda a demonstração comparativa (sem interação)
make teste      # executa a suíte de testes
make limpar     # remove os binários
```

Requisitos: `g++` com C++17 e `make`. Não há dependências externas.

## Os quatro métodos

Em todos eles `k` é a chave (o valor absoluto, para aceitar chaves negativas) e `m` é o
tamanho da tabela.

### 1. Divisibilidade

```
h(k) = k mod m
```

O mais simples e rápido. A qualidade depende de `m`: potências de 10 ou de 2 usam apenas os
dígitos/bits menos significativos, então o ideal é um número primo distante de potências de 2.
O menu sugere automaticamente o próximo primo ao redimensionar a tabela.

### 2. Multiplicidade (multiplicação)

```
h(k) = piso( m * parte_fracionária(k * A) ),  0 < A < 1
```

Usa por padrão `A = (raiz(5) - 1) / 2 ≈ 0,6180339887` (a constante sugerida por Knuth), e `A`
é configurável. A vantagem sobre a divisibilidade é não depender da escolha de `m`.

O cálculo é feito em **ponto fixo de 64 bits** (multiplicação 64x64 → 128 bits com metades de
32 bits) porque em `double` o produto `k * A` perde precisão para chaves acima de 2^53 —
justamente a faixa de CPFs, códigos de barras e afins. O resultado é idêntico ao da fórmula em
ponto flutuante para chaves pequenas, o que é verificado nos testes.

### 3. Dobra (folding)

Os dígitos da chave são quebrados em grupos, somados e o total é reduzido módulo `m`. O
tamanho do grupo é, por padrão, a quantidade de dígitos de `m - 1`, e pode ser configurado.

- **Dobra por deslocamento** (padrão): `12345678` → `678 + 345 + 12 = 1035`
- **Dobra na fronteira**: grupos alternados são lidos de trás para frente, como ao dobrar uma
  tira de papel → `678 + 543 + 12 = 1233`

É útil quando a chave é longa e todos os dígitos carregam informação.

### 4. Análise dos dígitos

Parte da observação de que, em conjuntos reais de chaves (matrículas, CPFs, códigos de
produto), algumas posições de dígito quase não variam. O método examina uma amostra de
chaves, calcula o **desvio padrão da distribuição dos dígitos 0–9 em cada posição** e escolhe
as posições mais uniformes — tantas quantas forem necessárias para endereçar a tabela. O
índice é formado apenas por esses dígitos.

```cpp
TabelaHash tabela(61, MetodoHash::AnaliseDigitos);
tabela.calibrarAnaliseDigitos(chaves);           // escolhe as posições
tabela.posicoesSelecionadas();                   // consulta as posições escolhidas
```

Sem calibração o método usa os dígitos menos significativos, o que equivale a
`k mod 10^d`. Trocar de método ou calibrar reconstrói a tabela automaticamente, sem perder
elementos.

## Exemplo de uso da classe

```cpp
#include "tabela_hash.hpp"

eda::TabelaHash tabela(eda::TabelaHash::proximoPrimo(20), eda::MetodoHash::Divisibilidade);

tabela.inserir(20250137, "Ana");
tabela.inserir(20250174, "Bruno");

if (const std::string* valor = tabela.buscar(20250137)) {
    // encontrado
}

tabela.remover(20250174);

tabela.definirMetodo(eda::MetodoHash::Dobra);    // reconstrói a tabela
eda::Estatisticas resumo = tabela.estatisticas();
```

Chaves textuais são aceitas via `TabelaHash::converterTextoParaChave("Ana")`, que reduz o
texto a um inteiro por hash polinomial antes de aplicar qualquer um dos quatro métodos.

## Operações e complexidade

| Operação | Caso médio | Pior caso |
| --- | --- | --- |
| `inserir` | O(1 + α) | O(n) |
| `buscar` / `contem` | O(1 + α) | O(n) |
| `remover` | O(1 + α) | O(n) |
| `definirMetodo` / `redimensionar` | O(n) | O(n) |
| `calibrarAnaliseDigitos` | O(a · d) | O(a · d) |

Sendo `α = n/m` o fator de carga, `a` o tamanho da amostra e `d` a quantidade de dígitos.

## Estatísticas

`estatisticas()` devolve tamanho da tabela, total de elementos, posições ocupadas e vazias,
número de colisões, maior cadeia, fator de carga e desvio padrão do comprimento das cadeias —
os dados usados pela opção "Comparar os quatro métodos" do menu:

```
Comparacao com 30 chaves em tabela de tamanho 61
Metodo                  Colisoes  Ocupadas  Maior cadeia      Desvio
--------------------------------------------------------------------
Divisibilidade                 0        30             1       0.500
Multiplicidade                 7        23             2       0.692
Dobra                         11        19             2       0.781
Analise dos digitos            0        30             1       0.500
```

Nenhum método é sempre melhor: o resultado depende do conjunto de chaves, e é exatamente
isso que a comparação permite observar.
