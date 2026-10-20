#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include <vector>
#include <string>
#include "Veiculo.h"
#include "FSM_Controlador.h"
#include "FilaEncadeada.h"
#include "Tiquete.h"
#include "Excecoes.h"

const int CAPACIDADE_VAGAS = 10;

/**
 * @brief Classe central orquestradora do Smart Parking Controller.
 * 
 * Integra:
 * - A Máquina de Estados da Cancela (FSM_Controlador);
 * - A Fila Encadeada Template de espera (FilaEncadeada<Veiculo*>);
 * - O pátio de vagas heterogêneo polimórfico (Veiculo* vagas[10]);
 * - O controle de exceções operacionais.
 */
class Estacionamento {
private:
    Veiculo* vagas[CAPACIDADE_VAGAS];
    FilaEncadeada<Veiculo*> filaEspera;
    FSM_Controlador fsmCancela;

    int proximoIdTiquete;
    int totalVeiculosAtendidos;
    double faturamentoTotal;

    int encontrarVagaLivre(int vagasNecessarias) const;
    int encontrarVagaPorPlaca(const std::string& placa) const;

public:
    Estacionamento();
    ~Estacionamento(); // RAII: Desaloca todos os veículos das vagas e da fila

    // Desabilita cópia rasa acidental
    Estacionamento(const Estacionamento&) = delete;
    Estacionamento& operator=(const Estacionamento&) = delete;

    /**
     * @brief Registra a aproximação de um veículo no sensor de entrada.
     * @param v Ponteiro para o veículo (alocado dinamicamente).
     */
    void chegarVeiculoEntrada(Veiculo* v);

    /**
     * @brief Pressiona o botão e emite o tíquete, alocando a vaga.
     * @param minutoAtual Hora simulada em minutos.
     * @throws EstacionamentoLotadoException se não houver vagas.
     * @throws FilaVaziaException se não houver veículo na fila.
     * @return Tiquete emitido.
     */
    Tiquete emitirTiquete(int minutoAtual);

    /**
     * @brief Confirma a passagem física do veículo pela cancela.
     * @return true se a travessia foi concluída e a cancela fechou.
     */
    bool confirmarTravessiaCancela();

    /**
     * @brief Processa o pagamento e saída de um veículo.
     * @param placa Placa do veículo a desocupar.
     * @param minutoSaida Hora de saída em minutos.
     * @return Valor pago pela permanência.
     */
    double registrarSaida(const std::string& placa, int minutoSaida);

    // Consultas
    int getVagasDisponiveis() const;
    int getVagasOcupadas() const;
    int getTamanhoFilaEspera() const { return filaEspera.tamanho(); }
    double getFaturamentoTotal() const { return faturamentoTotal; }
    int getTotalAtendidos() const { return totalVeiculosAtendidos; }
    std::string getEstadoCancela() const { return fsmCancela.getNomeEstadoAtual(); }
    int getCapacidade() const { return CAPACIDADE_VAGAS; }
    const Veiculo* getVeiculoVaga(int i) const {
        if (i >= 0 && i < CAPACIDADE_VAGAS) return vagas[i];
        return nullptr;
    }

    void exibirStatus() const;
    void exibirRelatorioFinanceiro() const;
};

#endif // ESTACIONAMENTO_H
