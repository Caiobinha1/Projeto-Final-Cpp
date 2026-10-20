#include "../include/Estacionamento.h"
#include <iostream>
#include <iomanip>

Estacionamento::Estacionamento()
    : proximoIdTiquete(1001), totalVeiculosAtendidos(0), faturamentoTotal(0.0) {
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        vagas[i] = nullptr;
    }
}

Estacionamento::~Estacionamento() {
    // Libera veículos estacionados nas vagas (RAII)
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr) {
            Veiculo* v = vagas[i];
            // Para caminhões que ocupam 2 vagas, limpa referências duplicadas antes do delete
            for (int j = i + 1; j < CAPACIDADE_VAGAS; ++j) {
                if (vagas[j] == v) {
                    vagas[j] = nullptr;
                }
            }
            delete v;
            vagas[i] = nullptr;
        }
    }

    // Libera veículos que eventualmente ficaram retidos na fila de espera
    while (!filaEspera.vazia()) {
        Veiculo* v = filaEspera.desenfileirar();
        delete v;
    }
}

int Estacionamento::encontrarVagaLivre(int vagasNecessarias) const {
    if (vagasNecessarias == 1) {
        for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
            if (vagas[i] == nullptr) return i;
        }
    } else if (vagasNecessarias == 2) {
        // Caminhões exigem duas vagas contíguas
        for (int i = 0; i < CAPACIDADE_VAGAS - 1; ++i) {
            if (vagas[i] == nullptr && vagas[i + 1] == nullptr) return i;
        }
    }
    return -1;
}

int Estacionamento::encontrarVagaPorPlaca(const std::string& placa) const {
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr && vagas[i]->getPlaca() == placa) {
            return i;
        }
    }
    return -1;
}

void Estacionamento::chegarVeiculoEntrada(Veiculo* v) {
    if (v == nullptr) return;

    if (fsmCancela.getEstadoAtual() == ESTADO_LIVRE) {
        fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
    }
    filaEspera.enfileirar(v);
}

Tiquete Estacionamento::emitirTiquete(int minutoAtual) {
    if (filaEspera.vazia()) {
        throw FilaVaziaException("Nenhum veiculo aguardando na cancela.");
    }

    Veiculo* v = filaEspera.frente();
    int vagaIdx = encontrarVagaLivre(v->getVagasNecessarias());

    if (vagaIdx == -1) {
        throw EstacionamentoLotadoException();
    }

    // Retira da fila de espera e aloca na(s) vaga(s)
    filaEspera.desenfileirar();
    if (v->getVagasNecessarias() == 1) {
        vagas[vagaIdx] = v;
    } else {
        vagas[vagaIdx] = v;
        vagas[vagaIdx + 1] = v;
    }

    // Atualiza FSM
    fsmCancela.processarEvento(EVENTO_BOTAO_PRESSIONADO);
    fsmCancela.processarEvento(EVENTO_TIQUETE_RETIRADO); // Cancela abre

    Tiquete t(proximoIdTiquete++, v->getPlaca(), v->getTipo(), minutoAtual, vagaIdx + 1);
    return t;
}

bool Estacionamento::confirmarTravessiaCancela() {
    bool ok = fsmCancela.processarEvento(EVENTO_SENSOR_PASSAGEM);
    if (ok) {
        // Conclui e reseta automaticamente para LIVRE para o próximo carro
        fsmCancela.processarEvento(EVENTO_SENSOR_PASSAGEM);
    }
    return ok;
}

double Estacionamento::registrarSaida(const std::string& placa, int minutoSaida) {
    int idx = encontrarVagaPorPlaca(placa);
    if (idx == -1) {
        throw TiqueteInvalidoException("Veiculo com placa '" + placa + "' nao encontrado no patio.");
    }

    Veiculo* v = vagas[idx];
    int tempo = minutoSaida - v->getMinutoEntrada();
    if (tempo < 0) tempo = 0;

    // Chamada polimórfica (Late binding via vtable)
    double tarifa = v->calcularTarifa(tempo);

    faturamentoTotal += tarifa;
    totalVeiculosAtendidos++;

    // Desaloca do pátio
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] == v) {
            vagas[i] = nullptr;
        }
    }

    delete v; // Destrutor virtual chamado corretamente
    return tarifa;
}

int Estacionamento::getVagasOcupadas() const {
    int ocupadas = 0;
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr) ocupadas++;
    }
    return ocupadas;
}

int Estacionamento::getVagasDisponiveis() const {
    return CAPACIDADE_VAGAS - getVagasOcupadas();
}

void Estacionamento::exibirStatus() const {
    std::cout << "\n--- MAPA DE OCUPAÇÃO DO ESTACIONAMENTO (" 
              << getVagasOcupadas() << "/" << CAPACIDADE_VAGAS << " VAGAS OCUPADAS) ---\n";
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        std::cout << "  Vaga " << std::setw(2) << i + 1 << ": ";
        if (vagas[i] == nullptr) {
            std::cout << "[LIVRE]\n";
        } else {
            std::cout << *vagas[i] << "\n";
        }
    }
    std::cout << "  Fila de espera na cancela: " << filaEspera.tamanho() << " veiculo(s)\n";
    std::cout << "  Estado da cancela: " << fsmCancela.getNomeEstadoAtual() << "\n";
    std::cout << "------------------------------------------------------------\n";
}

void Estacionamento::exibirRelatorioFinanceiro() const {
    std::cout << "\n============================================================\n";
    std::cout << "             RELATORIO FINANCEIRO E OPERACIONAL             \n";
    std::cout << "============================================================\n";
    std::cout << "  Total de Veiculos Atendidos: " << totalVeiculosAtendidos << "\n";
    std::cout << "  Total de Passagens pela Cancela: " << fsmCancela.getTotalPassagensSucesso() << "\n";
    std::cout << "  Total de Alertas de Seguranca: " << fsmCancela.getTotalAlertas() << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Faturamento Total Acumulado: R$ " << faturamentoTotal << "\n";
    std::cout << "============================================================\n";
}
