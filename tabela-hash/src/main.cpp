#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "tabela_hash.hpp"

using eda::Chave;
using eda::Estatisticas;
using eda::MetodoHash;
using eda::TabelaHash;

namespace {

const std::vector<MetodoHash> kMetodos = {MetodoHash::Divisibilidade, MetodoHash::Multiplicidade,
                                          MetodoHash::Dobra, MetodoHash::AnaliseDigitos};

std::vector<Chave> conjuntoDeExemplo() {
    // Matriculas ficticias: prefixo fixo de ano/curso e variacao no final,
    // caso classico em que a analise dos digitos faz diferenca.
    std::vector<Chave> chaves;
    for (int i = 0; i < 30; ++i) {
        chaves.push_back(20250100000LL + i * 37);
    }
    return chaves;
}

bool ehNumeroInteiro(const std::string& texto) {
    if (texto.empty()) {
        return false;
    }
    std::size_t inicio = (texto[0] == '-' || texto[0] == '+') ? 1 : 0;
    if (inicio >= texto.size()) {
        return false;
    }
    for (std::size_t i = inicio; i < texto.size(); ++i) {
        if (std::isdigit(static_cast<unsigned char>(texto[i])) == 0) {
            return false;
        }
    }
    return true;
}

Chave interpretarChave(const std::string& texto) {
    if (ehNumeroInteiro(texto)) {
        try {
            return std::stoll(texto);
        } catch (const std::exception&) {
            // Numero grande demais para long long: cai no hash textual.
        }
    }
    return TabelaHash::converterTextoParaChave(texto);
}

std::string lerLinha(const std::string& rotulo) {
    std::cout << rotulo;
    std::string linha;
    std::getline(std::cin, linha);
    return linha;
}

long long lerInteiro(const std::string& rotulo, long long padrao) {
    const std::string linha = lerLinha(rotulo);
    if (linha.empty()) {
        return padrao;
    }
    try {
        return std::stoll(linha);
    } catch (const std::exception&) {
        std::cout << "Valor invalido, usando " << padrao << ".\n";
        return padrao;
    }
}

void imprimirEstatisticas(const TabelaHash& tabela) {
    const Estatisticas resumo = tabela.estatisticas();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Metodo................: " << eda::nomeDoMetodo(tabela.metodo()) << "\n"
              << "Tamanho da tabela.....: " << resumo.tamanhoTabela << "\n"
              << "Elementos.............: " << resumo.totalElementos << "\n"
              << "Posicoes ocupadas.....: " << resumo.posicoesOcupadas << "\n"
              << "Posicoes vazias.......: " << resumo.posicoesVazias << "\n"
              << "Colisoes..............: " << resumo.colisoes << "\n"
              << "Maior cadeia..........: " << resumo.maiorCadeia << "\n"
              << "Fator de carga........: " << resumo.fatorDeCarga << "\n"
              << "Desvio das cadeias....: " << resumo.desvioPadraoDasCadeias << "\n";

    if (tabela.metodo() == MetodoHash::AnaliseDigitos) {
        std::cout << "Posicoes selecionadas.: ";
        if (tabela.posicoesSelecionadas().empty()) {
            std::cout << "(sem calibracao, usando digitos menos significativos)";
        } else {
            for (std::size_t posicao : tabela.posicoesSelecionadas()) {
                std::cout << posicao << " ";
            }
        }
        std::cout << "\n";
    }
}

void compararMetodos(const std::vector<Chave>& chaves, std::size_t tamanho) {
    std::cout << "\nComparacao com " << chaves.size() << " chaves em tabela de tamanho " << tamanho
              << "\n";
    std::cout << std::left << std::setw(22) << "Metodo" << std::right << std::setw(10) << "Colisoes"
              << std::setw(10) << "Ocupadas" << std::setw(14) << "Maior cadeia" << std::setw(12)
              << "Desvio" << "\n";
    std::cout << std::string(68, '-') << "\n";

    for (MetodoHash metodo : kMetodos) {
        TabelaHash tabela(tamanho, metodo);
        if (metodo == MetodoHash::AnaliseDigitos) {
            tabela.calibrarAnaliseDigitos(chaves);
        }
        for (Chave chave : chaves) {
            tabela.inserir(chave, "");
        }

        const Estatisticas resumo = tabela.estatisticas();
        std::cout << std::left << std::setw(22) << eda::nomeDoMetodo(metodo) << std::right
                  << std::setw(10) << resumo.colisoes << std::setw(10) << resumo.posicoesOcupadas
                  << std::setw(14) << resumo.maiorCadeia << std::setw(12) << std::fixed
                  << std::setprecision(3) << resumo.desvioPadraoDasCadeias << "\n";
    }
    std::cout << "\n";
}

void executarDemonstracao() {
    const std::vector<Chave> chaves = conjuntoDeExemplo();
    const std::size_t tamanho = TabelaHash::proximoPrimo(chaves.size() * 2);

    std::cout << "=== Demonstracao da tabela hash ===\n";
    compararMetodos(chaves, tamanho);

    TabelaHash tabela(11, MetodoHash::Divisibilidade);
    const std::vector<std::pair<Chave, std::string>> registros = {
        {102, "Ana"}, {217, "Bruno"}, {333, "Carla"}, {44, "Diego"}, {815, "Elisa"}};
    for (const auto& [chave, nome] : registros) {
        tabela.inserir(chave, nome);
    }

    for (MetodoHash metodo : kMetodos) {
        tabela.definirMetodo(metodo);
        if (metodo == MetodoHash::AnaliseDigitos) {
            tabela.calibrarAnaliseDigitos(tabela.chaves());
        }
        std::cout << tabela.paraTexto() << "\n";
    }

    std::cout << "Busca pela chave 333: " << (tabela.buscar(333) ? *tabela.buscar(333) : "nao encontrada")
              << "\n";
    tabela.remover(333);
    std::cout << "Apos remover 333, a tabela tem " << tabela.quantidade() << " elementos.\n";
}

void exibirMenu(const TabelaHash& tabela) {
    std::cout << "\n=== Tabela Hash | metodo atual: " << eda::nomeDoMetodo(tabela.metodo())
              << " | " << tabela.quantidade() << "/" << tabela.tamanho() << " ===\n"
              << " 1. Inserir\n"
              << " 2. Buscar\n"
              << " 3. Remover\n"
              << " 4. Exibir tabela\n"
              << " 5. Estatisticas\n"
              << " 6. Trocar metodo de hash\n"
              << " 7. Calibrar analise dos digitos (usa as chaves atuais)\n"
              << " 8. Carregar conjunto de exemplo\n"
              << " 9. Comparar os quatro metodos\n"
              << "10. Configurar parametros\n"
              << "11. Limpar tabela\n"
              << " 0. Sair\n"
              << "Opcao: ";
}

void trocarMetodo(TabelaHash& tabela) {
    std::cout << "1. Divisibilidade\n2. Multiplicidade\n3. Dobra\n4. Analise dos digitos\n";
    const long long opcao = lerInteiro("Escolha: ", 1);
    if (opcao < 1 || opcao > 4) {
        std::cout << "Opcao invalida.\n";
        return;
    }
    tabela.definirMetodo(kMetodos[static_cast<std::size_t>(opcao - 1)]);
    std::cout << "Metodo alterado para " << eda::nomeDoMetodo(tabela.metodo())
              << " (tabela reconstruida).\n";
}

void configurarParametros(TabelaHash& tabela) {
    std::cout << "1. Tamanho da tabela\n2. Constante A (multiplicidade)\n"
              << "3. Digitos por grupo (dobra)\n4. Alternar dobra na fronteira\n";
    const long long opcao = lerInteiro("Escolha: ", 0);

    switch (opcao) {
        case 1: {
            const long long novoTamanho = lerInteiro("Novo tamanho: ", 0);
            if (novoTamanho <= 0) {
                std::cout << "Tamanho invalido.\n";
                return;
            }
            const std::size_t primo = TabelaHash::proximoPrimo(static_cast<std::size_t>(novoTamanho));
            if (primo != static_cast<std::size_t>(novoTamanho)) {
                std::cout << "Sugestao: " << primo << " e primo e distribui melhor. Usando " << primo
                          << ".\n";
            }
            tabela.redimensionar(primo);
            break;
        }
        case 2: {
            const std::string linha = lerLinha("Constante A entre 0 e 1: ");
            try {
                tabela.definirConstanteMultiplicativa(std::stod(linha));
                std::cout << "Constante atualizada.\n";
            } catch (const std::exception& erro) {
                std::cout << "Valor invalido: " << erro.what() << "\n";
            }
            break;
        }
        case 3: {
            const long long digitos = lerInteiro("Digitos por grupo (0 = automatico): ", 0);
            tabela.definirDigitosPorGrupo(static_cast<std::size_t>(std::max(0LL, digitos)));
            std::cout << "Agrupamento atualizado.\n";
            break;
        }
        case 4: {
            static bool naFronteira = false;
            naFronteira = !naFronteira;
            tabela.definirDobraNaFronteira(naFronteira);
            std::cout << "Dobra na fronteira " << (naFronteira ? "ativada" : "desativada") << ".\n";
            break;
        }
        default:
            std::cout << "Opcao invalida.\n";
    }
}

void executarMenu() {
    TabelaHash tabela(TabelaHash::proximoPrimo(11), MetodoHash::Divisibilidade);

    while (true) {
        exibirMenu(tabela);

        std::string opcaoTexto;
        if (!std::getline(std::cin, opcaoTexto)) {
            return;
        }
        if (opcaoTexto.empty()) {
            continue;
        }

        long long opcao = -1;
        try {
            opcao = std::stoll(opcaoTexto);
        } catch (const std::exception&) {
            std::cout << "Opcao invalida.\n";
            continue;
        }

        switch (opcao) {
            case 1: {
                const std::string chaveTexto = lerLinha("Chave (numero ou texto): ");
                if (chaveTexto.empty()) {
                    break;
                }
                const Chave chave = interpretarChave(chaveTexto);
                const std::string valor = lerLinha("Valor: ");
                const bool nova = tabela.inserir(chave, valor);
                std::cout << (nova ? "Inserida" : "Atualizada") << " a chave " << chave
                          << " no indice " << tabela.indiceDe(chave) << ".\n";
                break;
            }
            case 2: {
                const Chave chave = interpretarChave(lerLinha("Chave: "));
                const std::string* valor = tabela.buscar(chave);
                if (valor != nullptr) {
                    std::cout << "Encontrada no indice " << tabela.indiceDe(chave) << ": " << *valor
                              << "\n";
                } else {
                    std::cout << "Chave nao encontrada.\n";
                }
                break;
            }
            case 3: {
                const Chave chave = interpretarChave(lerLinha("Chave: "));
                std::cout << (tabela.remover(chave) ? "Removida.\n" : "Chave nao encontrada.\n");
                break;
            }
            case 4:
                std::cout << tabela.paraTexto();
                break;
            case 5:
                imprimirEstatisticas(tabela);
                break;
            case 6:
                trocarMetodo(tabela);
                break;
            case 7: {
                const std::vector<Chave> chaves = tabela.chaves();
                if (chaves.empty()) {
                    std::cout << "Insira chaves antes de calibrar.\n";
                    break;
                }
                tabela.calibrarAnaliseDigitos(chaves);
                std::cout << "Calibrada. Posicoes escolhidas: ";
                for (std::size_t posicao : tabela.posicoesSelecionadas()) {
                    std::cout << posicao << " ";
                }
                std::cout << "\n";
                break;
            }
            case 8: {
                for (Chave chave : conjuntoDeExemplo()) {
                    tabela.inserir(chave, "matricula");
                }
                std::cout << "Conjunto de exemplo carregado.\n";
                break;
            }
            case 9: {
                std::vector<Chave> chaves = tabela.chaves();
                if (chaves.empty()) {
                    chaves = conjuntoDeExemplo();
                }
                compararMetodos(chaves, tabela.tamanho());
                break;
            }
            case 10:
                configurarParametros(tabela);
                break;
            case 11:
                tabela.limpar();
                std::cout << "Tabela limpa.\n";
                break;
            case 0:
                std::cout << "Ate mais!\n";
                return;
            default:
                std::cout << "Opcao invalida.\n";
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    const bool modoDemonstracao = argc > 1 && std::string(argv[1]) == "--demo";
    if (modoDemonstracao) {
        executarDemonstracao();
        return 0;
    }
    executarMenu();
    return 0;
}
