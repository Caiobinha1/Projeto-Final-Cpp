#ifndef EXCECOES_H
#define EXCECOES_H

#include <exception>
#include <string>

/**
 * @brief Classe base para todas as exceções do sistema de estacionamento.
 * Herda publicamente de std::exception conforme padrão canônico de C++.
 */
class EstacionamentoException : public std::exception {
protected:
    std::string mensagem;

public:
    explicit EstacionamentoException(const std::string& msg) : mensagem(msg) {}
    virtual ~EstacionamentoException() = default;

    virtual const char* what() const noexcept override {
        return mensagem.c_str();
    }
};

/**
 * @brief Lançada ao tentar desenfileirar ou consultar a frente de uma fila vazia.
 */
class FilaVaziaException : public EstacionamentoException {
public:
    FilaVaziaException() : EstacionamentoException("Erro: Operacao invalida sobre fila encadeada vazia.") {}
    explicit FilaVaziaException(const std::string& detalhe) : EstacionamentoException("Erro FilaVazia: " + detalhe) {}
};

/**
 * @brief Lançada quando o pátio atinge a capacidade máxima de ocupação.
 */
class EstacionamentoLotadoException : public EstacionamentoException {
public:
    EstacionamentoLotadoException() : EstacionamentoException("Erro: Estacionamento lotado. Nao ha vagas disponiveis.") {}
};

/**
 * @brief Lançada quando um tíquete apresentado não existe ou possui código inválido.
 */
class TiqueteInvalidoException : public EstacionamentoException {
public:
    TiqueteInvalidoException() : EstacionamentoException("Erro: Tiquete inexistente ou invalido no sistema.") {}
    explicit TiqueteInvalidoException(const std::string& codigo) 
        : EstacionamentoException("Erro: Tiquete com codigo '" + codigo + "' nao encontrado.") {}
};

#endif // EXCECOES_H
