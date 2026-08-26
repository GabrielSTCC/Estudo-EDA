#include "tabela_hash.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace eda {
namespace {

constexpr double kRazaoAurea = 0.6180339887498949;  // (sqrt(5) - 1) / 2
constexpr long double kDoisElevado64 = 18446744073709551616.0L;

unsigned long long moduloDaChave(Chave chave) {
    return chave < 0 ? static_cast<unsigned long long>(-(chave + 1)) + 1ULL
                     : static_cast<unsigned long long>(chave);
}

std::vector<unsigned> digitosDe(unsigned long long valor) {
    std::vector<unsigned> digitos;
    do {
        digitos.push_back(static_cast<unsigned>(valor % 10));
        valor /= 10;
    } while (valor > 0);
    return digitos;  // posição 0 = dígito menos significativo
}

std::size_t quantidadeDeDigitos(unsigned long long valor) {
    std::size_t total = 1;
    while (valor >= 10) {
        valor /= 10;
        ++total;
    }
    return total;
}

struct Produto128 {
    unsigned long long alto;
    unsigned long long baixo;
};

// Multiplicacao 64x64 -> 128 bits feita com metades de 32 bits, para nao
// depender da extensao __int128 nem perder precisao em double.
Produto128 multiplicar64(unsigned long long a, unsigned long long b) {
    constexpr unsigned long long kMascara = 0xFFFFFFFFULL;
    const unsigned long long aBaixo = a & kMascara;
    const unsigned long long aAlto = a >> 32;
    const unsigned long long bBaixo = b & kMascara;
    const unsigned long long bAlto = b >> 32;

    const unsigned long long baixoBaixo = aBaixo * bBaixo;
    const unsigned long long baixoAlto = aBaixo * bAlto;
    const unsigned long long altoBaixo = aAlto * bBaixo;
    const unsigned long long altoAlto = aAlto * bAlto;

    const unsigned long long meio =
        (baixoBaixo >> 32) + (baixoAlto & kMascara) + (altoBaixo & kMascara);

    Produto128 resultado;
    resultado.baixo = (meio << 32) | (baixoBaixo & kMascara);
    resultado.alto = altoAlto + (baixoAlto >> 32) + (altoBaixo >> 32) + (meio >> 32);
    return resultado;
}

double desvioPadrao(const std::vector<std::size_t>& amostra) {
    if (amostra.empty()) {
        return 0.0;
    }
    const double soma = std::accumulate(amostra.begin(), amostra.end(), 0.0,
                                        [](double acumulado, std::size_t valor) {
                                            return acumulado + static_cast<double>(valor);
                                        });
    const double media = soma / static_cast<double>(amostra.size());
    double variancia = 0.0;
    for (std::size_t valor : amostra) {
        const double diferenca = static_cast<double>(valor) - media;
        variancia += diferenca * diferenca;
    }
    return std::sqrt(variancia / static_cast<double>(amostra.size()));
}

}  // namespace

std::string nomeDoMetodo(MetodoHash metodo) {
    switch (metodo) {
        case MetodoHash::Divisibilidade:
            return "Divisibilidade";
        case MetodoHash::Multiplicidade:
            return "Multiplicidade";
        case MetodoHash::Dobra:
            return "Dobra";
        case MetodoHash::AnaliseDigitos:
            return "Analise dos digitos";
    }
    return "Desconhecido";
}

TabelaHash::TabelaHash(std::size_t tamanho, MetodoHash metodo)
    : tamanho_(tamanho == 0 ? 1 : tamanho),
      metodo_(metodo),
      constanteMultiplicativa_(kRazaoAurea),
      digitosPorGrupo_(0),
      dobraNaFronteira_(false),
      totalElementos_(0),
      colisoes_(0),
      baldes_(tamanho_) {}

std::size_t TabelaHash::digitosNecessarios() const {
    return quantidadeDeDigitos(static_cast<unsigned long long>(tamanho_ - 1));
}

std::size_t TabelaHash::hashDivisibilidade(unsigned long long chave) const {
    return static_cast<std::size_t>(chave % static_cast<unsigned long long>(tamanho_));
}

std::size_t TabelaHash::hashMultiplicidade(unsigned long long chave) const {
    // h(k) = piso(m * parte_fracionaria(k * A)) calculado em ponto fixo de 64
    // bits, porque em double o produto k * A perde precisao acima de 2^53.
    double fracaoDeA = constanteMultiplicativa_ - std::floor(constanteMultiplicativa_);
    const long double escalado = static_cast<long double>(fracaoDeA) * kDoisElevado64;
    unsigned long long constanteFixa =
        escalado >= kDoisElevado64 ? UINT64_MAX : static_cast<unsigned long long>(escalado);
    if (constanteFixa == 0) {
        return 0;
    }

    // Os 64 bits baixos do produto sao a parte fracionaria de k * A; escalar
    // por m e tomar os bits altos equivale a piso(m * frac(k * A)).
    const unsigned long long parteFracionaria = multiplicar64(chave, constanteFixa).baixo;
    const unsigned long long indice =
        multiplicar64(parteFracionaria, static_cast<unsigned long long>(tamanho_)).alto;
    return static_cast<std::size_t>(indice);
}

std::size_t TabelaHash::hashDobra(unsigned long long chave) const {
    const std::vector<unsigned> digitos = digitosDe(chave);
    const std::size_t porGrupo = digitosPorGrupo_ > 0 ? digitosPorGrupo_ : digitosNecessarios();

    unsigned long long soma = 0;
    std::size_t indiceDoGrupo = 0;
    for (std::size_t inicio = 0; inicio < digitos.size(); inicio += porGrupo) {
        const std::size_t fim = std::min(inicio + porGrupo, digitos.size());

        unsigned long long grupo = 0;
        if (dobraNaFronteira_ && indiceDoGrupo % 2 == 1) {
            // Dobra na fronteira: grupos alternados sao lidos de tras para frente.
            for (std::size_t i = inicio; i < fim; ++i) {
                grupo = grupo * 10 + digitos[i];
            }
        } else {
            for (std::size_t i = fim; i > inicio; --i) {
                grupo = grupo * 10 + digitos[i - 1];
            }
        }

        soma = (soma + grupo) % static_cast<unsigned long long>(tamanho_);
        ++indiceDoGrupo;
    }
    return static_cast<std::size_t>(soma);
}

std::size_t TabelaHash::hashAnaliseDigitos(unsigned long long chave) const {
    const std::vector<unsigned> digitos = digitosDe(chave);

    std::vector<std::size_t> posicoes = posicoesSelecionadas_;
    if (posicoes.empty()) {
        // Sem calibracao, usa os digitos menos significativos.
        const std::size_t necessarios = digitosNecessarios();
        posicoes.resize(necessarios);
        std::iota(posicoes.rbegin(), posicoes.rend(), std::size_t{0});
    }

    unsigned long long composto = 0;
    for (std::size_t posicao : posicoes) {
        const unsigned digito = posicao < digitos.size() ? digitos[posicao] : 0u;
        composto = (composto * 10 + digito) % static_cast<unsigned long long>(tamanho_);
    }
    return static_cast<std::size_t>(composto);
}

std::size_t TabelaHash::indiceDe(Chave chave) const {
    const unsigned long long valor = moduloDaChave(chave);
    switch (metodo_) {
        case MetodoHash::Divisibilidade:
            return hashDivisibilidade(valor);
        case MetodoHash::Multiplicidade:
            return hashMultiplicidade(valor);
        case MetodoHash::Dobra:
            return hashDobra(valor);
        case MetodoHash::AnaliseDigitos:
            return hashAnaliseDigitos(valor);
    }
    return hashDivisibilidade(valor);
}

bool TabelaHash::inserir(Chave chave, const std::string& valor) {
    std::list<Entrada>& balde = baldes_[indiceDe(chave)];

    for (Entrada& entrada : balde) {
        if (entrada.chave == chave) {
            entrada.valor = valor;
            return false;
        }
    }

    if (!balde.empty()) {
        ++colisoes_;
    }
    balde.push_back(Entrada{chave, valor});
    ++totalElementos_;
    return true;
}

bool TabelaHash::remover(Chave chave) {
    std::list<Entrada>& balde = baldes_[indiceDe(chave)];
    for (auto it = balde.begin(); it != balde.end(); ++it) {
        if (it->chave == chave) {
            balde.erase(it);
            --totalElementos_;
            if (!balde.empty()) {
                --colisoes_;
            }
            return true;
        }
    }
    return false;
}

const std::string* TabelaHash::buscar(Chave chave) const {
    const std::list<Entrada>& balde = baldes_[indiceDe(chave)];
    for (const Entrada& entrada : balde) {
        if (entrada.chave == chave) {
            return &entrada.valor;
        }
    }
    return nullptr;
}

bool TabelaHash::contem(Chave chave) const {
    return buscar(chave) != nullptr;
}

void TabelaHash::limpar() {
    baldes_.assign(tamanho_, std::list<Entrada>{});
    totalElementos_ = 0;
    colisoes_ = 0;
}

void TabelaHash::reconstruir() {
    std::vector<Entrada> entradas;
    entradas.reserve(totalElementos_);
    for (const std::list<Entrada>& balde : baldes_) {
        entradas.insert(entradas.end(), balde.begin(), balde.end());
    }

    limpar();
    for (const Entrada& entrada : entradas) {
        inserir(entrada.chave, entrada.valor);
    }
}

void TabelaHash::calibrarAnaliseDigitos(const std::vector<Chave>& amostra) {
    posicoesSelecionadas_.clear();
    if (amostra.empty()) {
        reconstruir();
        return;
    }

    std::size_t maiorQuantidadeDeDigitos = 1;
    for (Chave chave : amostra) {
        maiorQuantidadeDeDigitos =
            std::max(maiorQuantidadeDeDigitos, quantidadeDeDigitos(moduloDaChave(chave)));
    }

    // Para cada posicao de digito, mede o quanto a distribuicao de 0..9 se
    // afasta da uniforme; posicoes mais uniformes espalham melhor.
    std::vector<std::pair<double, std::size_t>> dispersaoPorPosicao;
    dispersaoPorPosicao.reserve(maiorQuantidadeDeDigitos);
    for (std::size_t posicao = 0; posicao < maiorQuantidadeDeDigitos; ++posicao) {
        std::vector<std::size_t> frequencias(10, 0);
        for (Chave chave : amostra) {
            const std::vector<unsigned> digitos = digitosDe(moduloDaChave(chave));
            const unsigned digito = posicao < digitos.size() ? digitos[posicao] : 0u;
            ++frequencias[digito];
        }
        dispersaoPorPosicao.emplace_back(desvioPadrao(frequencias), posicao);
    }

    std::stable_sort(dispersaoPorPosicao.begin(), dispersaoPorPosicao.end());

    const std::size_t desejadas = std::min(digitosNecessarios(), maiorQuantidadeDeDigitos);
    posicoesSelecionadas_.reserve(desejadas);
    for (std::size_t i = 0; i < desejadas; ++i) {
        posicoesSelecionadas_.push_back(dispersaoPorPosicao[i].second);
    }
    std::sort(posicoesSelecionadas_.rbegin(), posicoesSelecionadas_.rend());

    reconstruir();
}

const std::vector<std::size_t>& TabelaHash::posicoesSelecionadas() const {
    return posicoesSelecionadas_;
}

void TabelaHash::definirMetodo(MetodoHash metodo) {
    if (metodo_ == metodo) {
        return;
    }
    metodo_ = metodo;
    reconstruir();
}

void TabelaHash::definirConstanteMultiplicativa(double constante) {
    if (!(constante > 0.0) || !(constante < 1.0)) {
        throw std::invalid_argument("A constante multiplicativa deve estar entre 0 e 1.");
    }
    constanteMultiplicativa_ = constante;
    if (metodo_ == MetodoHash::Multiplicidade) {
        reconstruir();
    }
}

void TabelaHash::definirDigitosPorGrupo(std::size_t digitos) {
    digitosPorGrupo_ = digitos;
    if (metodo_ == MetodoHash::Dobra) {
        reconstruir();
    }
}

void TabelaHash::definirDobraNaFronteira(bool ativa) {
    dobraNaFronteira_ = ativa;
    if (metodo_ == MetodoHash::Dobra) {
        reconstruir();
    }
}

void TabelaHash::redimensionar(std::size_t novoTamanho) {
    tamanho_ = novoTamanho == 0 ? 1 : novoTamanho;

    std::vector<Entrada> entradas;
    entradas.reserve(totalElementos_);
    for (const std::list<Entrada>& balde : baldes_) {
        entradas.insert(entradas.end(), balde.begin(), balde.end());
    }

    baldes_.assign(tamanho_, std::list<Entrada>{});
    totalElementos_ = 0;
    colisoes_ = 0;
    for (const Entrada& entrada : entradas) {
        inserir(entrada.chave, entrada.valor);
    }
}

MetodoHash TabelaHash::metodo() const {
    return metodo_;
}

std::size_t TabelaHash::tamanho() const {
    return tamanho_;
}

std::size_t TabelaHash::quantidade() const {
    return totalElementos_;
}

bool TabelaHash::vazia() const {
    return totalElementos_ == 0;
}

const std::vector<std::list<Entrada>>& TabelaHash::baldes() const {
    return baldes_;
}

std::vector<Chave> TabelaHash::chaves() const {
    std::vector<Chave> resultado;
    resultado.reserve(totalElementos_);
    for (const std::list<Entrada>& balde : baldes_) {
        for (const Entrada& entrada : balde) {
            resultado.push_back(entrada.chave);
        }
    }
    return resultado;
}

Estatisticas TabelaHash::estatisticas() const {
    Estatisticas resumo;
    resumo.tamanhoTabela = tamanho_;
    resumo.totalElementos = totalElementos_;
    resumo.colisoes = colisoes_;
    resumo.fatorDeCarga = static_cast<double>(totalElementos_) / static_cast<double>(tamanho_);

    std::vector<std::size_t> comprimentos;
    comprimentos.reserve(tamanho_);
    for (const std::list<Entrada>& balde : baldes_) {
        comprimentos.push_back(balde.size());
        if (balde.empty()) {
            ++resumo.posicoesVazias;
        } else {
            ++resumo.posicoesOcupadas;
            resumo.maiorCadeia = std::max(resumo.maiorCadeia, balde.size());
        }
    }
    resumo.desvioPadraoDasCadeias = desvioPadrao(comprimentos);
    return resumo;
}

std::string TabelaHash::paraTexto() const {
    std::ostringstream saida;
    saida << "Tabela hash (" << nomeDoMetodo(metodo_) << ", tamanho " << tamanho_ << ")\n";
    for (std::size_t i = 0; i < baldes_.size(); ++i) {
        saida << "[" << i << "]";
        if (baldes_[i].empty()) {
            saida << " -";
        } else {
            for (const Entrada& entrada : baldes_[i]) {
                saida << " -> " << entrada.chave;
                if (!entrada.valor.empty()) {
                    saida << " (" << entrada.valor << ")";
                }
            }
        }
        saida << "\n";
    }
    return saida.str();
}

Chave TabelaHash::converterTextoParaChave(const std::string& texto) {
    // Hash polinomial simples para permitir chaves textuais nos quatro metodos.
    constexpr unsigned long long kBase = 31;
    constexpr unsigned long long kModulo = 1000000007ULL;

    unsigned long long acumulado = 0;
    for (unsigned char caractere : texto) {
        acumulado = (acumulado * kBase + caractere) % kModulo;
    }
    return static_cast<Chave>(acumulado);
}

bool TabelaHash::ehPrimo(std::size_t valor) {
    if (valor < 2) {
        return false;
    }
    if (valor % 2 == 0) {
        return valor == 2;
    }
    for (std::size_t divisor = 3; divisor * divisor <= valor; divisor += 2) {
        if (valor % divisor == 0) {
            return false;
        }
    }
    return true;
}

std::size_t TabelaHash::proximoPrimo(std::size_t valor) {
    std::size_t candidato = valor < 2 ? 2 : valor;
    while (!ehPrimo(candidato)) {
        ++candidato;
    }
    return candidato;
}

}  // namespace eda
