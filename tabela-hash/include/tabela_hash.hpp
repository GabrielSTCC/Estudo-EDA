#ifndef EDA_TABELA_HASH_HPP
#define EDA_TABELA_HASH_HPP

#include <cstddef>
#include <list>
#include <string>
#include <vector>

namespace eda {

using Chave = long long;

enum class MetodoHash {
    Divisibilidade,
    Multiplicidade,
    Dobra,
    AnaliseDigitos
};

std::string nomeDoMetodo(MetodoHash metodo);

struct Entrada {
    Chave chave;
    std::string valor;
};

struct Estatisticas {
    std::size_t tamanhoTabela = 0;
    std::size_t totalElementos = 0;
    std::size_t posicoesOcupadas = 0;
    std::size_t posicoesVazias = 0;
    std::size_t colisoes = 0;
    std::size_t maiorCadeia = 0;
    double fatorDeCarga = 0.0;
    double desvioPadraoDasCadeias = 0.0;
};

// Tabela hash com endereçamento fechado (encadeamento separado) e função de
// espalhamento selecionável entre os quatro métodos clássicos.
class TabelaHash {
public:
    TabelaHash(std::size_t tamanho, MetodoHash metodo);

    // Retorna false quando a chave já existe (o valor é atualizado mesmo assim).
    bool inserir(Chave chave, const std::string& valor);
    bool remover(Chave chave);
    const std::string* buscar(Chave chave) const;
    bool contem(Chave chave) const;
    void limpar();

    std::size_t indiceDe(Chave chave) const;

    // Escolhe as posições de dígito mais uniformes da amostra; sem calibração o
    // método usa os dígitos menos significativos.
    void calibrarAnaliseDigitos(const std::vector<Chave>& amostra);
    const std::vector<std::size_t>& posicoesSelecionadas() const;

    void definirMetodo(MetodoHash metodo);
    void definirConstanteMultiplicativa(double constante);
    void definirDigitosPorGrupo(std::size_t digitos);
    void definirDobraNaFronteira(bool ativa);
    void redimensionar(std::size_t novoTamanho);

    MetodoHash metodo() const;
    std::size_t tamanho() const;
    std::size_t quantidade() const;
    bool vazia() const;
    const std::vector<std::list<Entrada>>& baldes() const;
    std::vector<Chave> chaves() const;

    Estatisticas estatisticas() const;
    std::string paraTexto() const;

    static Chave converterTextoParaChave(const std::string& texto);
    static std::size_t proximoPrimo(std::size_t valor);
    static bool ehPrimo(std::size_t valor);

private:
    std::size_t hashDivisibilidade(unsigned long long chave) const;
    std::size_t hashMultiplicidade(unsigned long long chave) const;
    std::size_t hashDobra(unsigned long long chave) const;
    std::size_t hashAnaliseDigitos(unsigned long long chave) const;

    std::size_t digitosNecessarios() const;
    void reconstruir();

    std::size_t tamanho_;
    MetodoHash metodo_;
    double constanteMultiplicativa_;
    std::size_t digitosPorGrupo_;
    bool dobraNaFronteira_;
    std::vector<std::size_t> posicoesSelecionadas_;
    std::size_t totalElementos_;
    std::size_t colisoes_;
    std::vector<std::list<Entrada>> baldes_;
};

}  // namespace eda

#endif  // EDA_TABELA_HASH_HPP
