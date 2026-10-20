#ifndef FILA_ENCADEADA_H
#define FILA_ENCADEADA_H

#include <iostream>
#include "Excecoes.h"

/**
 * @brief Estrutura de Nó genérico para a fila encadeada dinâmica.
 * @tparam T Tipo de dado armazenado no nó.
 */
template <typename T>
struct Nodo {
    T dado;
    Nodo<T>* proximo;

    Nodo(const T& valor, Nodo<T>* prox = nullptr)
        : dado(valor), proximo(prox) {}
};

/**
 * @brief Fila encadeada dinâmica implementada via Template C++.
 * 
 * Atende os requisitos fundamentais do Prof. Bezerra:
 * 1. Alocação dinâmica manual de nós via new e delete;
 * 2. RAII: Destrutor garante liberação completa de memória;
 * 3. Deep Copy: Construtor de cópia e operador de atribuição sobrecarregados;
 * 4. Tratamento robusto de erros com exceções personalizadas.
 * 
 * @tparam T Tipo dos elementos enfileirados.
 */
template <typename T>
class FilaEncadeada {
private:
    Nodo<T>* primeiro;
    Nodo<T>* ultimo;
    int qtdElementos;

    void liberarTodos() {
        while (primeiro != nullptr) {
            Nodo<T>* temp = primeiro;
            primeiro = primeiro->proximo;
            delete temp;
        }
        ultimo = nullptr;
        qtdElementos = 0;
    }

    void copiarDe(const FilaEncadeada<T>& outra) {
        primeiro = nullptr;
        ultimo = nullptr;
        qtdElementos = 0;

        Nodo<T>* atual = outra.primeiro;
        while (atual != nullptr) {
            enfileirar(atual->dado);
            atual = atual->proximo;
        }
    }

public:
    // Construtor padrão
    FilaEncadeada() : primeiro(nullptr), ultimo(nullptr), qtdElementos(0) {}

    // Destrutor (RAII)
    ~FilaEncadeada() {
        liberarTodos();
    }

    // Construtor de Cópia (Deep Copy)
    FilaEncadeada(const FilaEncadeada<T>& outra) {
        copiarDe(outra);
    }

    // Sobrecarga do Operador de Atribuição (Deep Copy)
    FilaEncadeada<T>& operator=(const FilaEncadeada<T>& outra) {
        if (this != &outra) {
            liberarTodos();
            copiarDe(outra);
        }
        return *this;
    }

    /**
     * @brief Insere um novo elemento no final da fila (FIFO).
     * @param valor Elemento a ser inserido.
     */
    void enfileirar(const T& valor) {
        Nodo<T>* novo = new Nodo<T>(valor, nullptr);
        if (vazia()) {
            primeiro = novo;
            ultimo = novo;
        } else {
            ultimo->proximo = novo;
            ultimo = novo;
        }
        qtdElementos++;
    }

    /**
     * @brief Remove e retorna o elemento no início da fila.
     * @throws FilaVaziaException se a fila estiver vazia.
     * @return Elemento removido.
     */
    T desenfileirar() {
        if (vazia()) {
            throw FilaVaziaException("Tentativa de desenfileirar elemento em fila vazia.");
        }

        Nodo<T>* temp = primeiro;
        T dadoRetorno = temp->dado;

        primeiro = primeiro->proximo;
        if (primeiro == nullptr) {
            ultimo = nullptr;
        }

        delete temp;
        qtdElementos--;
        return dadoRetorno;
    }

    /**
     * @brief Retorna uma referência constante ao primeiro elemento sem removê-lo.
     * @throws FilaVaziaException se a fila estiver vazia.
     */
    const T& frente() const {
        if (vazia()) {
            throw FilaVaziaException("Consulta a frente() de uma fila vazia.");
        }
        return primeiro->dado;
    }

    bool vazia() const {
        return primeiro == nullptr;
    }

    int tamanho() const {
        return qtdElementos;
    }

    void limpar() {
        liberarTodos();
    }
};

#endif // FILA_ENCADEADA_H
