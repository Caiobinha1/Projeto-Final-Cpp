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
    virtual ~EstacionamentoException() noexcept = default;

    virtual const char* what() const noexcept override {
        return mensagem.c_str();
    }
};

/**
 * @brief Lançada ao tentar desenfileirar ou consultar a frente de uma fila vazia.
 */
class FilaVaziaException : public EstacionamentoException {
public:
    FilaVaziaException()
        : EstacionamentoException("Erro: Operacao invalida sobre fila encadeada vazia.") {}
    explicit FilaVaziaException(const std::string& detalhe)
        : EstacionamentoException("Erro FilaVazia: " + detalhe) {}
};

/**
 * @brief Lançada quando o pátio atinge a capacidade máxima de ocupação.
 */
class EstacionamentoLotadoException : public EstacionamentoException {
public:
    EstacionamentoLotadoException()
        : EstacionamentoException("Erro: Estacionamento lotado. Nao ha vagas disponiveis.") {}
    explicit EstacionamentoLotadoException(const std::string& detalhe)
        : EstacionamentoException("Erro EstacionamentoLotado: " + detalhe) {}
};

/**
 * @brief Lançada quando um tíquete ou placa apresentada não existe no sistema.
 */
class TiqueteInvalidoException : public EstacionamentoException {
public:
    TiqueteInvalidoException()
        : EstacionamentoException("Erro: Tiquete inexistente ou invalido no sistema.") {}
    explicit TiqueteInvalidoException(const std::string& codigo)
        : EstacionamentoException("Erro: Tiquete/Placa '" + codigo + "' nao encontrado no patio.") {}
};

/**
 * @brief Lançada quando a cancela está em estado de alerta ou já aberta aguardando passagem.
 */
class CancelaBloqueadaException : public EstacionamentoException {
public:
    explicit CancelaBloqueadaException(const std::string& detalhe)
        : EstacionamentoException("Erro CancelaBloqueada: " + detalhe) {}
};

/**
 * @brief Lançada ao tentar registrar um veículo com placa já presente no pátio ou na fila.
 */
class PlacaDuplicadaException : public EstacionamentoException {
public:
    explicit PlacaDuplicadaException(const std::string& placa)
        : EstacionamentoException("Erro: Veiculo com placa '" + placa + "' ja esta registrado no sistema.") {}
};

#endif // EXCECOES_H
