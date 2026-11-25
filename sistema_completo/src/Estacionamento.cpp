#include "../include/Estacionamento.h"
#include "../include/UtilitariosTemplate.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

Estacionamento::Estacionamento(IHardwareCancela* hardwareExterno)
    : relogioSistema(24, 11, 2026, 8, 0),
      hal(hardwareExterno),
      possuiHalPropria(hardwareExterno == nullptr),
      proximoIdTiquete(1001),
      totalVeiculosAtendidos(0),
      faturamentoTotal(0.0) {
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        vagas[i] = nullptr;
    }

    if (possuiHalPropria) {
        hal = new HardwareSimulado();
    }

    atendidosPorCategoria["Carro"] = 0;
    atendidosPorCategoria["Moto"] = 0;
    atendidosPorCategoria["Caminhao"] = 0;
    faturamentoPorCategoria["Carro"] = 0.0;
    faturamentoPorCategoria["Moto"] = 0.0;
    faturamentoPorCategoria["Caminhao"] = 0.0;

    hal->acionarMotorCancela(false);
    hal->acionarSemaforo(false);
    hal->acionarAlarmeSonoro(false);
    atualizarDisplayPadraoLocked();
}

Estacionamento::~Estacionamento() {
    pararWatchdog();

    std::lock_guard<std::mutex> lock(mutexSistema);
    // Libera veículos estacionados nas vagas (RAII + destrutor virtual)
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr) {
            Veiculo* v = vagas[i];
            for (int j = i + 1; j < CAPACIDADE_VAGAS; ++j) {
                if (vagas[j] == v) {
                    vagas[j] = nullptr;
                }
            }
            delete v;
            vagas[i] = nullptr;
        }
    }

    // Libera veículos que eventualmente ficaram retidos na fila de espera (noexcept-safe)
    try {
        while (!filaEspera.vazia()) {
            Veiculo* v = filaEspera.desenfileirar();
            delete v;
        }
    } catch (...) {
        // Destrutor nunca propaga exceções
    }

    if (possuiHalPropria && hal != nullptr) {
        delete hal;
        hal = nullptr;
    }
}

int Estacionamento::encontrarVagaLivreLocked(int vagasNecessarias) const {
    if (vagasNecessarias == 1) {
        for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
            if (vagas[i] == nullptr) return i;
        }
    } else if (vagasNecessarias == 2) {
        for (int i = 0; i < CAPACIDADE_VAGAS - 1; ++i) {
            if (vagas[i] == nullptr && vagas[i + 1] == nullptr) return i;
        }
    }
    return -1;
}

int Estacionamento::encontrarVagaPorPlacaLocked(const std::string& placa) const {
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr && vagas[i]->getPlaca() == placa) {
            return i;
        }
    }
    return -1;
}

int Estacionamento::contarVagasOcupadasLocked() const {
    int ocupadas = 0;
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] != nullptr) ocupadas++;
    }
    return ocupadas;
}

bool Estacionamento::placaJaRegistradaLocked(const std::string& placa) const {
    if (encontrarVagaPorPlacaLocked(placa) != -1) {
        return true;
    }
    // Verifica cópia segura da fila (exercitando Deep Copy de FilaEncadeada<T>)
    FilaEncadeada<Veiculo*> copiaFila(filaEspera);
    while (!copiaFila.vazia()) {
        Veiculo* item = copiaFila.desenfileirar();
        if (item != nullptr && item->getPlaca() == placa) {
            return true;
        }
    }
    return false;
}

void Estacionamento::atualizarDisplayPadraoLocked() {
    if (hal == nullptr) return;
    int livres = CAPACIDADE_VAGAS - contarVagasOcupadasLocked();
    if (fsmCancela.isEmAlerta()) {
        hal->atualizarDisplayLCD("ALERTA TIMEOUT!", "DESOBSTRUA VAO");
    } else if (fsmCancela.isCancelaAberta()) {
        hal->atualizarDisplayLCD("CANCELA ABERTA", "PROSSIGA P/ VAGA");
    } else if (fsmCancela.getEstadoAtual() == ESTADO_VEICULO_DETECTADO) {
        hal->atualizarDisplayLCD("VEICULO DETECT.", "PRESSIONE BOTAO");
    } else {
        std::ostringstream l2;
        l2 << "VAGAS: " << livres << " LIVRES";
        hal->atualizarDisplayLCD("CANCELA PRONTA", l2.str());
    }
}

void Estacionamento::chegarVeiculoEntrada(Veiculo* v) {
    if (v == nullptr) return;

    std::lock_guard<std::mutex> lock(mutexSistema);
    if (placaJaRegistradaLocked(v->getPlaca())) {
        std::string placaDup = v->getPlaca();
        delete v; // Evita vazamento de memória ao rejeitar duplicata
        throw PlacaDuplicadaException(placaDup);
    }

    relogioSistema.sincronizarMinutoDia(v->getMinutoEntrada());
    filaEspera.enfileirar(v);

    if (fsmCancela.getEstadoAtual() == ESTADO_LIVRE) {
        fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
        atualizarDisplayPadraoLocked();
    }
}

Tiquete Estacionamento::emitirTiquete(int minutoAtual) {
    std::lock_guard<std::mutex> lock(mutexSistema);

    if (fsmCancela.isEmAlerta()) {
        throw CancelaBloqueadaException("Cancela em ALERTA_BLOQUEIO. Execute o reset de seguranca.");
    }
    if (fsmCancela.isCancelaAberta()) {
        throw CancelaBloqueadaException("Cancela ja esta aberta aguardando a travessia do veiculo anterior.");
    }
    if (filaEspera.vazia()) {
        throw FilaVaziaException("Nenhum veiculo aguardando na cancela.");
    }

    // Garante que a FSM esteja pronta para o veículo da frente
    if (fsmCancela.getEstadoAtual() == ESTADO_LIVRE) {
        fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
    }

    Veiculo* v = filaEspera.frente();
    int vagaIdx = encontrarVagaLivreLocked(v->getVagasNecessarias());

    if (vagaIdx == -1) {
        if (v->getVagasNecessarias() == 2) {
            throw EstacionamentoLotadoException("Nao ha 2 vagas contiguas disponiveis para caminhao.");
        }
        throw EstacionamentoLotadoException();
    }

    // Sincroniza relógio e atualiza horário real de entrada no pátio
    relogioSistema.sincronizarMinutoDia(minutoAtual);
    v->setMinutoEntrada(minutoAtual);

    // Retira da fila de espera e aloca na(s) vaga(s)
    filaEspera.desenfileirar();
    if (v->getVagasNecessarias() == 1) {
        vagas[vagaIdx] = v;
    } else {
        vagas[vagaIdx] = v;
        vagas[vagaIdx + 1] = v;
    }

    // Atualiza FSM (VEICULO_DETECTADO -> EMITINDO_TIQUETE -> CANCELA_ABERTA)
    fsmCancela.processarEvento(EVENTO_BOTAO_PRESSIONADO);
    fsmCancela.processarEvento(EVENTO_TIQUETE_RETIRADO);

    // Aciona periféricos via HAL
    if (hal != nullptr) {
        hal->acionarMotorCancela(true);
        hal->acionarSemaforo(true);
        hal->acionarAlarmeSonoro(false);
        std::ostringstream l2;
        if (v->getVagasNecessarias() == 2) {
            l2 << "VAGAS " << (vagaIdx + 1) << "-" << (vagaIdx + 2) << " OK";
        } else {
            l2 << "VAGA " << (vagaIdx + 1) << " LIBERADA";
        }
        hal->atualizarDisplayLCD("CANCELA ABERTA", l2.str());
    }

    Tiquete t(proximoIdTiquete++,
              v->getPlaca(),
              v->getTipo(),
              minutoAtual,
              vagaIdx + 1,
              relogioSistema.formatarDataHora());
    return t;
}

bool Estacionamento::confirmarTravessiaCancela() {
    std::lock_guard<std::mutex> lock(mutexSistema);
    bool ok = fsmCancela.processarEvento(EVENTO_SENSOR_PASSAGEM);
    if (ok) {
        // Conclui passagem e fecha cancela (PASSAGEM_CONCLUIDA -> LIVRE)
        fsmCancela.processarEvento(EVENTO_SENSOR_PASSAGEM);

        if (hal != nullptr) {
            hal->acionarMotorCancela(false);
            hal->acionarSemaforo(false);
        }

        // Se ainda houver veículos aguardando na fila, o próximo avança para o laço indutivo
        if (!filaEspera.vazia()) {
            fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
        }
        atualizarDisplayPadraoLocked();
    }
    return ok;
}

bool Estacionamento::recuarVeiculoEntrada() {
    std::lock_guard<std::mutex> lock(mutexSistema);
    if (filaEspera.vazia() || fsmCancela.getEstadoAtual() != ESTADO_VEICULO_DETECTADO) {
        return false;
    }

    Veiculo* desistente = filaEspera.desenfileirar();
    delete desistente;

    bool ok = fsmCancela.processarEvento(EVENTO_VEICULO_RECUOU);
    if (ok && !filaEspera.vazia()) {
        fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
    }
    atualizarDisplayPadraoLocked();
    return ok;
}

bool Estacionamento::simularTimeoutCancela() {
    std::lock_guard<std::mutex> lock(mutexSistema);
    bool ok = fsmCancela.processarEvento(EVENTO_TIMEOUT_PASSAGEM);
    if (ok && hal != nullptr) {
        hal->acionarSemaforo(false);
        hal->acionarAlarmeSonoro(true);
        atualizarDisplayPadraoLocked();
    }
    return ok;
}

bool Estacionamento::resetarAlertaCancela() {
    std::lock_guard<std::mutex> lock(mutexSistema);
    bool ok = fsmCancela.processarEvento(EVENTO_RESET_SISTEMA);
    if (ok) {
        if (hal != nullptr) {
            hal->acionarMotorCancela(false);
            hal->acionarSemaforo(false);
            hal->acionarAlarmeSonoro(false);
        }
        if (!filaEspera.vazia()) {
            fsmCancela.processarEvento(EVENTO_VEICULO_CHEGOU);
        }
        atualizarDisplayPadraoLocked();
    }
    return ok;
}

double Estacionamento::registrarSaida(const std::string& placa, int minutoSaida) {
    std::lock_guard<std::mutex> lock(mutexSistema);
    int idx = encontrarVagaPorPlacaLocked(placa);
    if (idx == -1) {
        throw TiqueteInvalidoException(placa);
    }

    relogioSistema.sincronizarMinutoDia(minutoSaida);

    Veiculo* v = vagas[idx];
    int tempo = minutoSaida - v->getMinutoEntrada();
    if (tempo < 0) tempo = 0;

    // Chamada polimórfica (Late binding via vtable)
    double tarifa = v->calcularTarifa(tempo);
    std::string categoria = v->getTipo();
    std::string modelo = v->getModelo();
    int minEnt = v->getMinutoEntrada();

    faturamentoTotal += tarifa;
    totalVeiculosAtendidos++;
    atendidosPorCategoria[categoria]++;
    faturamentoPorCategoria[categoria] += tarifa;

    RegistroMovimentacao reg;
    reg.idRegistro = static_cast<int>(historico.size()) + 1;
    reg.placa = placa;
    reg.modelo = modelo;
    reg.tipo = categoria;
    reg.dataHoraSaida = relogioSistema.formatarDataHora();
    reg.minutoEntrada = minEnt;
    reg.minutoSaida = minutoSaida;
    reg.permanenciaMinutos = tempo;
    reg.valorPago = tarifa;
    historico.push_back(reg);

    // Desaloca do pátio (1 ou 2 vagas contíguas)
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        if (vagas[i] == v) {
            vagas[i] = nullptr;
        }
    }

    delete v; // Destrutor virtual chamado corretamente
    atualizarDisplayPadraoLocked();
    return tarifa;
}

void Estacionamento::avancarRelogio(int minutos) {
    std::lock_guard<std::mutex> lock(mutexSistema);
    relogioSistema += minutos;
}

void Estacionamento::sincronizarRelogio(int minutoAbsoluto) {
    std::lock_guard<std::mutex> lock(mutexSistema);
    relogioSistema.sincronizarMinutoDia(minutoAbsoluto);
}

std::string Estacionamento::getDataHoraAtual() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return relogioSistema.formatarDataHora();
}

int Estacionamento::getMinutoAtualSistema() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return relogioSistema.getMinutosAcumulados();
}

void Estacionamento::iniciarWatchdog(int timeoutMs) {
    pararWatchdog();
    watchdog.reset(new WatchdogCancela(
        &fsmCancela,
        &mutexSistema,
        timeoutMs,
        15,
        [this]() {
            std::lock_guard<std::mutex> lock(mutexSistema);
            if (hal != nullptr) {
                hal->acionarSemaforo(false);
                hal->acionarAlarmeSonoro(true);
                atualizarDisplayPadraoLocked();
            }
        }
    ));
    watchdog->iniciar();
}

void Estacionamento::pararWatchdog() {
    if (watchdog) {
        watchdog->parar();
    }
}

int Estacionamento::getDisparosWatchdog() const {
    if (!watchdog) return 0;
    return watchdog->getDisparosTimeout();
}

bool Estacionamento::exportarHistoricoCSV(const std::string& caminhoArquivo) const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::ofstream arq(caminhoArquivo.c_str());
    if (!arq.is_open()) {
        return false;
    }

    arq << "id,placa,modelo,tipo,data_hora_saida,minuto_entrada,minuto_saida,permanencia_min,valor_pago\n";
    arq << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < historico.size(); ++i) {
        const RegistroMovimentacao& r = historico[i];
        arq << r.idRegistro << ","
            << r.placa << ","
            << r.modelo << ","
            << r.tipo << ","
            << r.dataHoraSaida << ","
            << r.minutoEntrada << ","
            << r.minutoSaida << ","
            << r.permanenciaMinutos << ","
            << r.valorPago << "\n";
    }
    arq.close();
    return true;
}

bool Estacionamento::salvarRelatorioTXT(const std::string& caminhoArquivo) const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::ofstream arq(caminhoArquivo.c_str());
    if (!arq.is_open()) {
        return false;
    }

    std::vector<double> valores;
    valores.reserve(historico.size());
    for (std::size_t i = 0; i < historico.size(); ++i) {
        valores.push_back(historico[i].valorPago);
    }
    double ticketMedio = calcularMedia(valores);

    arq << "============================================================\n";
    arq << "  SMART PARKING CONTROLLER - RELATORIO GERENCIAL (UFSC)\n";
    arq << "============================================================\n";
    arq << "Data/Hora do Sistema: " << relogioSistema << "\n";
    arq << "Plataforma HAL:       " << (hal ? hal->getNomePlataforma() : "N/A") << "\n";
    arq << "Vagas Ocupadas:       " << contarVagasOcupadasLocked() << "/" << CAPACIDADE_VAGAS << "\n";
    arq << "Fila na Cancela:      " << filaEspera.tamanho() << "\n";
    arq << "Estado da FSM:        " << fsmCancela.getNomeEstadoAtual() << "\n";
    arq << "Passagens com Sucesso:" << fsmCancela.getTotalPassagensSucesso() << "\n";
    arq << "Alertas de Timeout:   " << fsmCancela.getTotalAlertas() << "\n";
    arq << "------------------------------------------------------------\n";
    arq << std::fixed << std::setprecision(2);
    arq << "Resumo por Categoria (STL std::map):\n";
    for (std::map<std::string, int>::const_iterator it = atendidosPorCategoria.begin();
         it != atendidosPorCategoria.end(); ++it) {
        double fatCat = 0.0;
        std::map<std::string, double>::const_iterator itFat = faturamentoPorCategoria.find(it->first);
        if (itFat != faturamentoPorCategoria.end()) {
            fatCat = itFat->second;
        }
        arq << "  - " << std::setw(10) << std::left << it->first << std::right
            << ": " << it->second << " veiculo(s) | R$ " << fatCat << "\n";
    }
    arq << "------------------------------------------------------------\n";
    arq << "Total Atendidos:      " << totalVeiculosAtendidos << "\n";
    arq << "Ticket Medio:         R$ " << ticketMedio << "\n";
    arq << "Faturamento Total:    R$ " << faturamentoTotal << "\n";
    arq << "============================================================\n";
    arq.close();
    return true;
}

int Estacionamento::carregarContagemRegistrosCSV(const std::string& caminhoArquivo) {
    std::ifstream arq(caminhoArquivo.c_str());
    if (!arq.is_open()) {
        return -1;
    }
    std::string linha;
    int contagem = 0;
    // Ignora cabeçalho
    if (std::getline(arq, linha)) {
        while (std::getline(arq, linha)) {
            if (!linha.empty()) {
                contagem++;
            }
        }
    }
    arq.close();
    return contagem;
}

int Estacionamento::getVagasOcupadas() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return contarVagasOcupadasLocked();
}

int Estacionamento::getVagasDisponiveis() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return CAPACIDADE_VAGAS - contarVagasOcupadasLocked();
}

int Estacionamento::getTamanhoFilaEspera() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return filaEspera.tamanho();
}

double Estacionamento::getFaturamentoTotal() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return faturamentoTotal;
}

double Estacionamento::getTicketMedio() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::vector<double> valores;
    valores.reserve(historico.size());
    for (std::size_t i = 0; i < historico.size(); ++i) {
        valores.push_back(historico[i].valorPago);
    }
    return calcularMedia(valores);
}

int Estacionamento::getMaiorPermanenciaMinutos() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    if (historico.empty()) return 0;
    std::vector<RegistroMovimentacao>::const_iterator it = std::max_element(
        historico.begin(),
        historico.end(),
        [](const RegistroMovimentacao& a, const RegistroMovimentacao& b) {
            return a.permanenciaMinutos < b.permanenciaMinutos;
        }
    );
    return it->permanenciaMinutos;
}

int Estacionamento::getTotalAtendidos() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return totalVeiculosAtendidos;
}

int Estacionamento::getAtendidosCategoria(const std::string& categoria) const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::map<std::string, int>::const_iterator it = atendidosPorCategoria.find(categoria);
    return (it != atendidosPorCategoria.end()) ? it->second : 0;
}

double Estacionamento::getFaturamentoCategoria(const std::string& categoria) const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::map<std::string, double>::const_iterator it = faturamentoPorCategoria.find(categoria);
    return (it != faturamentoPorCategoria.end()) ? it->second : 0.0;
}

std::string Estacionamento::getEstadoCancela() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return fsmCancela.getNomeEstadoAtual();
}

int Estacionamento::getTotalPassagensCancela() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return fsmCancela.getTotalPassagensSucesso();
}

int Estacionamento::getTotalAlertasCancela() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    return fsmCancela.getTotalAlertas();
}

const Veiculo* Estacionamento::getVeiculoVaga(int i) const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    if (i >= 0 && i < CAPACIDADE_VAGAS) return vagas[i];
    return nullptr;
}

void Estacionamento::exibirStatus() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::cout << "\n--- MAPA DE OCUPACAO DO ESTACIONAMENTO ("
              << contarVagasOcupadasLocked() << "/" << CAPACIDADE_VAGAS << " VAGAS OCUPADAS) ---\n";
    std::cout << "  Data/Hora: " << relogioSistema << "\n";
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        std::cout << "  Vaga " << std::setw(2) << i + 1 << ": ";
        if (vagas[i] == nullptr) {
            std::cout << "[LIVRE]\n";
        } else {
            std::cout << *vagas[i] << "\n";
        }
    }
    std::cout << "  Fila de espera na cancela: " << filaEspera.tamanho() << " veiculo(s)\n";
    std::cout << "  Estado da cancela (FSM):   " << fsmCancela.getNomeEstadoAtual() << "\n";
    if (hal != nullptr) {
        std::cout << "  HAL Display LCD 16x2:      [" << hal->getDisplayLinha1()
                  << " | " << hal->getDisplayLinha2() << "] Servo: "
                  << hal->getAnguloServo() << " deg\n";
    }
    std::cout << "------------------------------------------------------------\n";
}

void Estacionamento::exibirRelatorioFinanceiro() const {
    std::lock_guard<std::mutex> lock(mutexSistema);
    std::vector<double> valores;
    valores.reserve(historico.size());
    for (std::size_t i = 0; i < historico.size(); ++i) {
        valores.push_back(historico[i].valorPago);
    }
    double ticketMedio = calcularMedia(valores);

    std::cout << "\n============================================================\n";
    std::cout << "             RELATORIO FINANCEIRO E OPERACIONAL             \n";
    std::cout << "============================================================\n";
    std::cout << "  Data/Hora Atual:                " << relogioSistema << "\n";
    std::cout << "  Total de Veiculos Atendidos:    " << totalVeiculosAtendidos << "\n";
    std::cout << "  Total de Passagens pela Cancela:" << fsmCancela.getTotalPassagensSucesso() << "\n";
    std::cout << "  Total de Alertas de Seguranca:  " << fsmCancela.getTotalAlertas() << "\n";
    std::cout << std::fixed << std::setprecision(2);
    for (std::map<std::string, int>::const_iterator it = atendidosPorCategoria.begin();
         it != atendidosPorCategoria.end(); ++it) {
        double fatCat = 0.0;
        std::map<std::string, double>::const_iterator itFat = faturamentoPorCategoria.find(it->first);
        if (itFat != faturamentoPorCategoria.end()) fatCat = itFat->second;
        std::cout << "    - " << std::setw(10) << std::left << it->first << std::right
                  << ": " << it->second << " un. | R$ " << fatCat << "\n";
    }
    std::cout << "  Ticket Medio:                   R$ " << ticketMedio << "\n";
    std::cout << "  Faturamento Total Acumulado:    R$ " << faturamentoTotal << "\n";
    std::cout << "============================================================\n";
}
