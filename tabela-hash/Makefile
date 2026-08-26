CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude

BIN_DIR := bin
FONTE_TABELA := src/tabela_hash.cpp
PROGRAMA := $(BIN_DIR)/tabela_hash
TESTES := $(BIN_DIR)/testes

.PHONY: all executar demo teste limpar

all: $(PROGRAMA) $(TESTES)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(PROGRAMA): src/main.cpp $(FONTE_TABELA) include/tabela_hash.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) src/main.cpp $(FONTE_TABELA) -o $@

$(TESTES): tests/teste_tabela_hash.cpp $(FONTE_TABELA) include/tabela_hash.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) tests/teste_tabela_hash.cpp $(FONTE_TABELA) -o $@

executar: $(PROGRAMA)
	./$(PROGRAMA)

demo: $(PROGRAMA)
	./$(PROGRAMA) --demo

teste: $(TESTES)
	./$(TESTES)

limpar:
	rm -rf $(BIN_DIR)
