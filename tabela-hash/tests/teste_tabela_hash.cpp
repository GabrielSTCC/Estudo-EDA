#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "tabela_hash.hpp"

using eda::Chave;
using eda::Estatisticas;
using eda::MetodoHash;
using eda::TabelaHash;

namespace {

int totalDeTestes = 0;
int totalDeFalhas = 0;

void verificar(bool condicao, const std::string& descricao) {
    ++totalDeTestes;
    if (condicao) {
        std::cout << "[ ok ] " << descricao << "\n";
    } else {
        ++totalDeFalhas;
        std::cout << "[FALHA] " << descricao << "\n";
    }
}

void testarDivisibilidade() {
    TabelaHash tabela(11, MetodoHash::Divisibilidade);
    verificar(tabela.indiceDe(25) == 3, "divisibilidade: 25 mod 11 = 3");
    verificar(tabela.indiceDe(11) == 0, "divisibilidade: 11 mod 11 = 0");
    verificar(tabela.indiceDe(-25) == 3, "divisibilidade: chave negativa usa o modulo");

    for (Chave chave = 0; chave < 500; ++chave) {
        if (tabela.indiceDe(chave) >= tabela.tamanho()) {
            verificar(false, "divisibilidade: indice fora da faixa");
            return;
        }
    }
    verificar(true, "divisibilidade: indices sempre dentro da faixa");
}

void testarMultiplicidade() {
    const std::size_t tamanho = 16384;
    TabelaHash tabela(tamanho, MetodoHash::Multiplicidade);

    bool dentroDaFaixa = true;
    bool equivaleAoCalculoEmPontoFlutuante = true;
    const double constante = 0.6180339887498949;
    for (Chave chave = 1; chave <= 5000; ++chave) {
        const std::size_t indice = tabela.indiceDe(chave);
        dentroDaFaixa = dentroDaFaixa && indice < tamanho;

        const double produto = static_cast<double>(chave) * constante;
        const std::size_t esperado =
            static_cast<std::size_t>(static_cast<double>(tamanho) * (produto - std::floor(produto)));
        // Tolerancia de 1 posicao: o ponto fixo arredonda de forma levemente
        // diferente do calculo em ponto flutuante nas bordas.
        const std::size_t diferenca = indice > esperado ? indice - esperado : esperado - indice;
        equivaleAoCalculoEmPontoFlutuante = equivaleAoCalculoEmPontoFlutuante && diferenca <= 1;
    }
    verificar(dentroDaFaixa, "multiplicidade: indices sempre dentro da faixa");
    verificar(equivaleAoCalculoEmPontoFlutuante,
              "multiplicidade: equivale a piso(m * frac(k * A))");

    verificar(tabela.indiceDe(123456) == tabela.indiceDe(123456),
              "multiplicidade: funcao deterministica");

    const std::size_t antes = tabela.indiceDe(98765);
    tabela.definirConstanteMultiplicativa(0.3141592653589793);
    verificar(tabela.indiceDe(98765) != antes, "multiplicidade: constante A altera o espalhamento");

    bool lancouExcecao = false;
    try {
        tabela.definirConstanteMultiplicativa(1.5);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    verificar(lancouExcecao, "multiplicidade: rejeita constante fora do intervalo (0,1)");
}

void testarDobra() {
    TabelaHash tabela(1000, MetodoHash::Dobra);
    // 12.345.678 -> grupos 678 + 345 + 12 = 1035 -> 1035 mod 1000 = 35
    verificar(tabela.indiceDe(12345678) == 35, "dobra por deslocamento: 678+345+12 = 1035 -> 35");

    tabela.definirDobraNaFronteira(true);
    // Grupos alternados invertidos: 678 + 543 + 12 = 1233 -> 233
    verificar(tabela.indiceDe(12345678) == 233, "dobra na fronteira: 678+543+12 = 1233 -> 233");

    tabela.definirDobraNaFronteira(false);
    tabela.definirDigitosPorGrupo(2);
    // Grupos de 2: 78 + 56 + 34 + 12 = 180
    verificar(tabela.indiceDe(12345678) == 180, "dobra: agrupamento configuravel de 2 digitos");

    TabelaHash pequena(97, MetodoHash::Dobra);
    bool dentroDaFaixa = true;
    for (Chave chave = 0; chave < 100000; chave += 7) {
        dentroDaFaixa = dentroDaFaixa && pequena.indiceDe(chave) < pequena.tamanho();
    }
    verificar(dentroDaFaixa, "dobra: indices sempre dentro da faixa");
}

void testarAnaliseDigitos() {
    // Chaves com prefixo constante 777 e ultimo digito uniforme: a analise deve
    // descartar as posicoes constantes e escolher a posicao 0.
    std::vector<Chave> amostra;
    for (Chave i = 0; i < 10; ++i) {
        amostra.push_back(7770 + i);
    }

    TabelaHash tabela(10, MetodoHash::AnaliseDigitos);
    verificar(tabela.posicoesSelecionadas().empty(),
              "analise dos digitos: sem calibracao nao ha posicoes escolhidas");

    tabela.calibrarAnaliseDigitos(amostra);
    verificar(tabela.posicoesSelecionadas().size() == 1,
              "analise dos digitos: escolhe um digito para tabela de tamanho 10");
    verificar(tabela.posicoesSelecionadas().front() == 0,
              "analise dos digitos: escolhe a posicao mais uniforme (0)");

    for (Chave chave : amostra) {
        tabela.inserir(chave, "");
    }
    const Estatisticas resumo = tabela.estatisticas();
    verificar(resumo.colisoes == 0, "analise dos digitos: distribui 10 chaves sem colisao");
    verificar(resumo.posicoesOcupadas == 10, "analise dos digitos: ocupa todas as posicoes");

    // Comparacao com a divisibilidade em chaves cujo sufixo se repete.
    std::vector<Chave> comSufixoRepetido;
    for (Chave i = 0; i < 20; ++i) {
        comSufixoRepetido.push_back(1000 * i + 50);
    }
    TabelaHash porAnalise(13, MetodoHash::AnaliseDigitos);
    porAnalise.calibrarAnaliseDigitos(comSufixoRepetido);
    for (Chave chave : comSufixoRepetido) {
        porAnalise.inserir(chave, "");
    }
    verificar(porAnalise.estatisticas().posicoesOcupadas > 1,
              "analise dos digitos: evita concentrar chaves de sufixo repetido");
}

void testarOperacoesBasicas() {
    TabelaHash tabela(7, MetodoHash::Divisibilidade);

    verificar(tabela.vazia(), "operacoes: tabela inicia vazia");
    verificar(tabela.inserir(10, "dez"), "operacoes: insercao de chave nova retorna true");
    verificar(!tabela.inserir(10, "DEZ"), "operacoes: chave repetida atualiza e retorna false");
    verificar(tabela.quantidade() == 1, "operacoes: chave repetida nao duplica elemento");

    const std::string* valor = tabela.buscar(10);
    verificar(valor != nullptr && *valor == "DEZ", "operacoes: busca devolve o valor atualizado");
    verificar(tabela.buscar(99) == nullptr, "operacoes: busca de chave ausente devolve nulo");
    verificar(tabela.contem(10) && !tabela.contem(99), "operacoes: contem responde corretamente");

    tabela.inserir(17, "dezessete");  // 17 mod 7 = 3, mesma posicao de 10
    verificar(tabela.indiceDe(10) == tabela.indiceDe(17), "operacoes: colisao provocada no indice 3");
    verificar(tabela.estatisticas().colisoes == 1, "operacoes: colisao contabilizada");

    verificar(tabela.remover(10), "operacoes: remocao de chave existente");
    verificar(!tabela.remover(10), "operacoes: remocao repetida retorna false");
    verificar(tabela.estatisticas().colisoes == 0, "operacoes: colisao descontada apos remocao");
    verificar(tabela.contem(17), "operacoes: encadeamento preserva a outra chave do balde");

    tabela.limpar();
    verificar(tabela.vazia() && tabela.quantidade() == 0, "operacoes: limpar esvazia a tabela");
}

void testarTrocaDeMetodoERedimensionamento() {
    TabelaHash tabela(11, MetodoHash::Divisibilidade);
    const std::vector<Chave> chaves = {12345, 67890, 24680, 13579, 11111, 99999};
    for (Chave chave : chaves) {
        tabela.inserir(chave, "v" + std::to_string(chave));
    }

    for (MetodoHash metodo : {MetodoHash::Multiplicidade, MetodoHash::Dobra,
                              MetodoHash::AnaliseDigitos, MetodoHash::Divisibilidade}) {
        tabela.definirMetodo(metodo);

        bool todasPresentes = true;
        bool indicesCoerentes = true;
        for (Chave chave : chaves) {
            todasPresentes = todasPresentes && tabela.contem(chave);
            const std::size_t indice = tabela.indiceDe(chave);
            indicesCoerentes = indicesCoerentes && indice < tabela.tamanho();
        }
        verificar(todasPresentes && tabela.quantidade() == chaves.size(),
                  "troca de metodo (" + eda::nomeDoMetodo(metodo) + "): reconstroi sem perder chaves");
        verificar(indicesCoerentes,
                  "troca de metodo (" + eda::nomeDoMetodo(metodo) + "): indices validos");
    }

    tabela.redimensionar(31);
    bool preservou = tabela.tamanho() == 31 && tabela.quantidade() == chaves.size();
    for (Chave chave : chaves) {
        preservou = preservou && tabela.contem(chave);
    }
    verificar(preservou, "redimensionamento: mantem todas as chaves acessiveis");

    const Estatisticas resumo = tabela.estatisticas();
    verificar(resumo.colisoes == resumo.totalElementos - resumo.posicoesOcupadas,
              "estatisticas: colisoes = elementos - posicoes ocupadas");
    verificar(resumo.posicoesOcupadas + resumo.posicoesVazias == resumo.tamanhoTabela,
              "estatisticas: ocupadas + vazias = tamanho da tabela");
}

void testarUtilitarios() {
    verificar(TabelaHash::ehPrimo(97) && !TabelaHash::ehPrimo(1) && !TabelaHash::ehPrimo(100),
              "utilitarios: teste de primalidade");
    verificar(TabelaHash::proximoPrimo(60) == 61, "utilitarios: proximo primo de 60 e 61");
    verificar(TabelaHash::proximoPrimo(97) == 97, "utilitarios: proximo primo de um primo e ele mesmo");

    const Chave primeira = TabelaHash::converterTextoParaChave("estrutura de dados");
    const Chave segunda = TabelaHash::converterTextoParaChave("estrutura de dados");
    verificar(primeira == segunda && primeira >= 0, "utilitarios: chave textual estavel e nao negativa");
    verificar(TabelaHash::converterTextoParaChave("abc") != TabelaHash::converterTextoParaChave("acb"),
              "utilitarios: chave textual sensivel a ordem dos caracteres");

    TabelaHash tabela(13, MetodoHash::Dobra);
    tabela.inserir(TabelaHash::converterTextoParaChave("Ana"), "aluna");
    verificar(tabela.contem(TabelaHash::converterTextoParaChave("Ana")),
              "utilitarios: chave textual funciona na tabela");
}

void testarChavesGrandes() {
    // Chaves acima de 2^53 quebrariam o calculo da multiplicidade em double.
    TabelaHash tabela(1009, MetodoHash::Multiplicidade);
    const std::vector<Chave> chaves = {9007199254740993LL, 9007199254740994LL, 4611686018427387904LL};

    bool todasValidas = true;
    for (Chave chave : chaves) {
        tabela.inserir(chave, "grande");
        todasValidas = todasValidas && tabela.indiceDe(chave) < tabela.tamanho() && tabela.contem(chave);
    }
    verificar(todasValidas, "chaves grandes: multiplicidade continua estavel acima de 2^53");

    for (MetodoHash metodo : {MetodoHash::Dobra, MetodoHash::AnaliseDigitos,
                              MetodoHash::Divisibilidade}) {
        tabela.definirMetodo(metodo);
        for (Chave chave : chaves) {
            todasValidas = todasValidas && tabela.contem(chave);
        }
    }
    verificar(todasValidas, "chaves grandes: acessiveis em todos os metodos");
}

}  // namespace

int main() {
    std::cout << "=== Testes da tabela hash ===\n";
    testarDivisibilidade();
    testarMultiplicidade();
    testarDobra();
    testarAnaliseDigitos();
    testarOperacoesBasicas();
    testarTrocaDeMetodoERedimensionamento();
    testarUtilitarios();
    testarChavesGrandes();

    std::cout << "\n" << (totalDeTestes - totalDeFalhas) << "/" << totalDeTestes
              << " verificacoes passaram.\n";
    return totalDeFalhas == 0 ? 0 : 1;
}
