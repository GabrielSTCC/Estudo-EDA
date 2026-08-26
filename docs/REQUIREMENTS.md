# Requisitos — Estudo-EDA

Documento de requisitos do repositório de estudos de Estruturas de Dados e Algoritmos.

## Requisitos Funcionais

### RF-001 — Tabela hash com função de espalhamento selecionável

O sistema deve oferecer uma tabela hash em C++ cuja função de espalhamento possa ser
escolhida na criação e trocada em tempo de execução entre os métodos de divisibilidade,
multiplicidade, dobra e análise dos dígitos. A troca de método deve reconstruir a tabela
redistribuindo todos os elementos já armazenados, sem perda de dados.

Implementação: `tabela-hash/include/tabela_hash.hpp`, `tabela-hash/src/tabela_hash.cpp`.

### RF-002 — Método da divisibilidade

O índice deve ser calculado por `h(k) = k mod m`, sendo `k` o valor absoluto da chave e `m`
o tamanho da tabela. O sistema deve disponibilizar um utilitário de próximo número primo e
sugeri-lo ao usuário quando o tamanho informado não for primo.

### RF-003 — Método da multiplicidade

O índice deve ser calculado por `h(k) = piso(m * parte_fracionária(k * A))`, com `A` padrão
igual a `(raiz(5) - 1) / 2` e configurável no intervalo aberto `(0, 1)`. Valores de `A` fora
desse intervalo devem ser rejeitados com erro. O cálculo deve permanecer correto para chaves
acima de 2^53, faixa em que a aritmética em ponto flutuante perderia precisão.

### RF-004 — Método da dobra

A chave deve ser dividida em grupos de dígitos, somada e reduzida módulo `m`. O tamanho do
grupo é, por padrão, a quantidade de dígitos de `m - 1` e deve ser configurável. Devem existir
duas variantes: dobra por deslocamento (padrão) e dobra na fronteira, na qual grupos
alternados são lidos na ordem inversa.

### RF-005 — Método da análise dos dígitos

O sistema deve permitir calibrar o método a partir de uma amostra de chaves, medindo o desvio
padrão da distribuição dos dígitos 0–9 em cada posição e selecionando as posições mais
uniformes, em quantidade suficiente para endereçar a tabela. As posições escolhidas devem
poder ser consultadas. Sem calibração, o método deve usar os dígitos menos significativos.

### RF-006 — Operações da tabela e tratamento de colisões

A tabela deve suportar inserção, busca, remoção, verificação de existência, limpeza e
redimensionamento, preservando os elementos ao redimensionar. As colisões devem ser tratadas
por encadeamento separado. A inserção de chave já existente deve atualizar o valor sem
duplicar o elemento.

### RF-007 — Chaves textuais

Além de chaves numéricas inteiras, o sistema deve aceitar chaves em texto, convertidas para
inteiro por hash polinomial antes da aplicação de qualquer um dos quatro métodos.

### RF-008 — Estatísticas de distribuição

A tabela deve expor tamanho, total de elementos, posições ocupadas e vazias, número de
colisões, maior cadeia, fator de carga e desvio padrão do comprimento das cadeias, permitindo
comparar objetivamente a qualidade dos métodos de espalhamento.

### RF-009 — Programa de demonstração

Deve haver um executável com menu interativo para inserir, buscar, remover, exibir a tabela,
ver estatísticas, trocar o método, calibrar a análise dos dígitos, carregar um conjunto de
exemplo, comparar os quatro métodos e ajustar parâmetros. O mesmo executável deve oferecer um
modo não interativo (`--demo`) com a comparação entre os métodos.

Implementação: `tabela-hash/src/main.cpp`.

## Requisitos Não Funcionais

### RNF-001 — Portabilidade e dependências

O código deve compilar com `g++` em C++17 sem dependências externas e sem avisos com
`-Wall -Wextra -Wpedantic`. Não devem ser usadas extensões de compilador não padronizadas.

### RNF-002 — Automação de build e testes

O projeto deve ter um `Makefile` com alvos para compilar, executar, rodar a demonstração,
rodar os testes e limpar os binários. Os testes devem ser executáveis sem framework externo e
devolver código de saída diferente de zero em caso de falha.

Implementação: `tabela-hash/Makefile`, `tabela-hash/tests/teste_tabela_hash.cpp`.

### RNF-003 — Idioma

Código, documentação e mensagens ao usuário devem estar em português.

## Histórico de Versão

| Data | Versão | Alteração |
| --- | --- | --- |
| 26/08/2026 | 1.0 | Criação do documento com os requisitos RF-001 a RF-009 e RNF-001 a RNF-003, referentes à implementação da tabela hash em C++ com os métodos de divisibilidade, multiplicidade, dobra e análise dos dígitos. |
