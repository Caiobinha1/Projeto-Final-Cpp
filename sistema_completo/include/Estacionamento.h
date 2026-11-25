#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include "Veiculo.h"
#include "FSM_Controlador.h"
#include "FilaEncadeada.h"
#include "Tiquete.h"
#include "Excecoes.h"
#include "RelogioCalendario.h"
#include "HardwareHAL.h"
#include "WatchdogCancela.h"

const int CAPACIDADE_VAGAS = 10;

/**
 * @brief Registro imutável de uma movimentação encerrada para auditoria (STL + Arquivos).
 */
struct RegistroMovimentacao {
    int idRegistro;
    std::string placa;
    std::string modelo;
    std::string tipo;
    std::string dataHoraSaida;
    int minutoEntrada;
    int minutoSaida;
    int permanenciaMinutos;
    double valorPago;
};

/**
 * @brief Classe central orquestradora do Smart Parking Controller.
 *
 * Integra todos os pilares da disciplina EEL7323 / EEL510265:
 * - Máquina de Estados Finitos determinística (FSM_Controlador);
 * - Estrutura de dados dinâmica própria com Template (FilaEncadeada<Veiculo*>);
 * - Pátio heterogêneo polimórfico (Veiculo* vagas[10]) com destrutor virtual e RAII;
 * - Herança múltipla e sobrecarga de operadores (RelogioCalendario);
 * - Camada de Abstração de Hardware via interface abstrata (IHardwareCancela);
 * - Contêineres e algoritmos da STL (std::vector, std::map, <algorithm>);
 * - Persistência e leitura de arquivos (<fstream>);
 * - Monitoramento concorrente thread-safe (WatchdogCancela com std::thread + std::mutex);
 * - Hierarquia completa de exceções customizadas (std::exception).
 */
class Estacionamento {
private:
    Veiculo* vagas[CAPACIDADE_VAGAS];
    FilaEncadeada<Veiculo*> filaEspera;
    FSM_Controlador fsmCancela;
    RelogioCalendario relogioSistema;

    IHardwareCancela* hal;
    bool possuiHalPropria;

    std::vector<RegistroMovimentacao> historico;
    std::map<std::string, int> atendidosPorCategoria;
    std::map<std::string, double> faturamentoPorCategoria;

    mutable std::mutex mutexSistema;
    std::unique_ptr<WatchdogCancela> watchdog;

    int proximoIdTiquete;
    int totalVeiculosAtendidos;
    double faturamentoTotal;

    int encontrarVagaLivreLocked(int vagasNecessarias) const;
    int encontrarVagaPorPlacaLocked(const std::string& placa) const;
    int contarVagasOcupadasLocked() const;
    bool placaJaRegistradaLocked(const std::string& placa) const;
    void atualizarDisplayPadraoLocked();

public:
    explicit Estacionamento(IHardwareCancela* hardwareExterno = nullptr);
    ~Estacionamento(); // RAII: para watchdog e desaloca vagas, fila e HAL própria

    // Desabilita cópia rasa acidental
    Estacionamento(const Estacionamento&) = delete;
    Estacionamento& operator=(const Estacionamento&) = delete;

    // --- Operações principais ---
    void chegarVeiculoEntrada(Veiculo* v);
    Tiquete emitirTiquete(int minutoAtual);
    bool confirmarTravessiaCancela();
    bool recuarVeiculoEntrada();
    bool simularTimeoutCancela();
    bool resetarAlertaCancela();
    double registrarSaida(const std::string& placa, int minutoSaida);

    // --- Controle de relógio (Herança Múltipla) ---
    void avancarRelogio(int minutos);
    void sincronizarRelogio(int minutoAbsoluto);
    std::string getDataHoraAtual() const;
    int getMinutoAtualSistema() const;

    // --- Watchdog concorrente (std::thread + std::mutex) ---
    void iniciarWatchdog(int timeoutMs = 10000);
    void pararWatchdog();
    int getDisparosWatchdog() const;

    // --- Persistência em Arquivo (<fstream>) ---
    bool exportarHistoricoCSV(const std::string& caminhoArquivo) const;
    bool salvarRelatorioTXT(const std::string& caminhoArquivo) const;
    static int carregarContagemRegistrosCSV(const std::string& caminhoArquivo);

    // --- Consultas e Estatísticas (STL + Templates) ---
    int getVagasDisponiveis() const;
    int getVagasOcupadas() const;
    int getTamanhoFilaEspera() const;
    double getFaturamentoTotal() const;
    double getTicketMedio() const;
    int getMaiorPermanenciaMinutos() const;
    int getTotalAtendidos() const;
    int getAtendidosCategoria(const std::string& categoria) const;
    double getFaturamentoCategoria(const std::string& categoria) const;
    std::string getEstadoCancela() const;
    int getTotalPassagensCancela() const;
    int getTotalAlertasCancela() const;
    int getCapacidade() const { return CAPACIDADE_VAGAS; }

    const Veiculo* getVeiculoVaga(int i) const;
    const IHardwareCancela* getHardware() const { return hal; }
    const std::vector<RegistroMovimentacao>& getHistorico() const { return historico; }

    void exibirStatus() const;
    void exibirRelatorioFinanceiro() const;
};

#endif // ESTACIONAMENTO_H
