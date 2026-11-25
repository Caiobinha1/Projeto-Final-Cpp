#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdio>
#include "../include/Estacionamento.h"
#include "../include/Carro.h"
#include "../include/Moto.h"
#include "../include/Caminhao.h"
#include "../include/Excecoes.h"
#include "../include/UtilitariosTemplate.h"

using namespace std;

static bool quaseIgual(double a, double b, double eps = 1e-6) {
    return fabs(a - b) < eps;
}

static string escaparJson(const string& s) {
    string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '"' || s[i] == '\\') {
            out += '\\';
        }
        out += s[i];
    }
    return out;
}

void executarSuiteTestesAutomatizados() {
    cout << "=================================================================\n";
    cout << "  UFSC / CTC / EEL - EEL7323 Programacao C++ para Embarcados     \n";
    cout << "  PROJETO FINAL INTEGRADO - SMART PARKING CONTROLLER             \n";
    cout << "  BATERIA COMPLETA DE TESTES AUTOMATIZADOS (10 SUITES)           \n";
    cout << "=================================================================\n\n";

    Estacionamento est;

    // Teste 1: Estado Inicial + HAL
    cout << "[TESTE 1] Condicoes iniciais do estacionamento e camada HAL...\n";
    assert(est.getVagasDisponiveis() == CAPACIDADE_VAGAS);
    assert(est.getVagasOcupadas() == 0);
    assert(est.getTamanhoFilaEspera() == 0);
    assert(quaseIgual(est.getFaturamentoTotal(), 0.0));
    assert(est.getTotalAtendidos() == 0);
    assert(est.getEstadoCancela() == "LIVRE");
    assert(est.getHardware()->getAnguloServo() == 0);
    assert(!est.getHardware()->isLedVerdeAceso());
    cout << "  -> Patio, FSM e HAL (" << est.getHardware()->getNomePlataforma() << ") inicializados. [SUCESSO]\n\n";

    // Teste 2: Entrada, Fila com Multiplos Veiculos e Sincronismo da FSM
    cout << "[TESTE 2] Chegada simultanea na fila (3 categorias) e sincronismo FSM...\n";
    est.chegarVeiculoEntrada(new Carro("CAR-1001", "VW Polo", 480, false));
    est.chegarVeiculoEntrada(new Moto("MOT-2002", "Honda CB500", 480, 500));
    est.chegarVeiculoEntrada(new Caminhao("CAM-3003", "Volvo FH", 480, 3));
    assert(est.getTamanhoFilaEspera() == 3);
    assert(est.getEstadoCancela() == "VEICULO_DETECTADO");

    Tiquete t1 = est.emitirTiquete(480);
    cout << "  Emitido: " << t1 << endl;
    assert(t1.getPlaca() == "CAR-1001");
    assert(t1.getTipoVeiculo() == "Carro");
    assert(t1.getNumeroVaga() == 1);
    assert(est.getEstadoCancela() == "CANCELA_ABERTA");
    assert(est.getHardware()->getAnguloServo() == 90);
    assert(est.getHardware()->isLedVerdeAceso());

    // Tentativa de emitir novo tiquete antes de fechar a cancela deve lançar CancelaBloqueadaException
    bool capturouAberta = false;
    try {
        est.emitirTiquete(481);
    } catch (const CancelaBloqueadaException& e) {
        capturouAberta = true;
        cout << "  -> Bloqueio com cancela aberta capturado: " << e.what() << endl;
    }
    assert(capturouAberta);

    bool travessiaOk = est.confirmarTravessiaCancela();
    assert(travessiaOk);
    (void)travessiaOk;
    // Como ha mais veiculos na fila, a FSM deve avancar automaticamente para VEICULO_DETECTADO
    assert(est.getEstadoCancela() == "VEICULO_DETECTADO");
    assert(est.getHardware()->getAnguloServo() == 0);

    Tiquete t2 = est.emitirTiquete(495);
    cout << "  Emitido: " << t2 << endl;
    assert(t2.getPlaca() == "MOT-2002");
    assert(t2.getNumeroVaga() == 2);
    travessiaOk = est.confirmarTravessiaCancela();
    assert(travessiaOk);
    assert(est.getEstadoCancela() == "VEICULO_DETECTADO");

    Tiquete t3 = est.emitirTiquete(510);
    cout << "  Emitido: " << t3 << endl;
    assert(t3.getPlaca() == "CAM-3003");
    assert(t3.getNumeroVaga() == 3);
    travessiaOk = est.confirmarTravessiaCancela();
    assert(travessiaOk);
    assert(est.getEstadoCancela() == "LIVRE"); // Fila esvaziou

    assert(est.getVagasOcupadas() == 4); // 1 Carro + 1 Moto + 2 Caminhao
    assert(est.getVagasDisponiveis() == 6);
    cout << "  -> Fila multipla, sincronismo FSM e alocacao de vagas contiguas validados! [SUCESSO]\n\n";

    est.exibirStatus();

    // Teste 3: Saidas, Polimorfismo Dinamico (vtable) e Estatisticas STL
    cout << "\n[TESTE 3] Tarifacao polimorfica (vtable) e metricas STL (vector/map/algorithm)...\n";
    // Carro: 480 -> 630 = 150 min (1h base 10 + 2h extras * 3 = 16.00)
    double valorCarro = est.registrarSaida("CAR-1001", 630);
    cout << "  Saida Carro CAR-1001 (150 min): R$ " << fixed << setprecision(2) << valorCarro << endl;
    assert(quaseIgual(valorCarro, 16.0));

    // Moto: 495 -> 505 = 10 min (tolerancia <= 15 min = 0.00)
    double valorMoto = est.registrarSaida("MOT-2002", 505);
    cout << "  Saida Moto MOT-2002 (10 min - Tolerancia): R$ " << valorMoto << endl;
    assert(quaseIgual(valorMoto, 0.0));

    // Caminhao 3 eixos: 510 -> 600 = 90 min (1h base 15 + 1h extra 5 + 1 eixo extra 2.50 = 22.50)
    double valorCaminhao = est.registrarSaida("CAM-3003", 600);
    cout << "  Saida Caminhao CAM-3003 (90 min, 3 eixos): R$ " << valorCaminhao << endl;
    assert(quaseIgual(valorCaminhao, 22.50));

    // Carro com Tag Eletronica (10% desconto) e Moto > 500cc (+2.00)
    est.chegarVeiculoEntrada(new Carro("TAG-4004", "Honda Civic", 600, true));
    est.emitirTiquete(600);
    est.confirmarTravessiaCancela();
    double valorTag = est.registrarSaida("TAG-4004", 750); // 150 min -> 16.0 * 0.9 = 14.40
    assert(quaseIgual(valorTag, 14.40));

    est.chegarVeiculoEntrada(new Moto("BIG-5005", "Yamaha MT-07", 600, 689));
    est.emitirTiquete(600);
    est.confirmarTravessiaCancela();
    double valorBigMoto = est.registrarSaida("BIG-5005", 720); // 120 min -> 5 + 1.5 + 2 = 8.50
    assert(quaseIgual(valorBigMoto, 8.50));

    assert(est.getVagasOcupadas() == 0);
    assert(est.getTotalAtendidos() == 5);
    assert(est.getAtendidosCategoria("Carro") == 2);
    assert(est.getAtendidosCategoria("Moto") == 2);
    assert(est.getAtendidosCategoria("Caminhao") == 1);
    assert(est.getMaiorPermanenciaMinutos() == 150);
    assert(quaseIgual(est.getFaturamentoTotal(), 16.0 + 0.0 + 22.50 + 14.40 + 8.50));
    assert(quaseIgual(est.getTicketMedio(), (16.0 + 0.0 + 22.50 + 14.40 + 8.50) / 5.0));
    cout << "  -> Polimorfismo, STL std::map/std::vector e template calcularMedia validados! [SUCESSO]\n\n";

    // Teste 4: Heranca Multipla (RelogioCalendario) e Sobrecarga de Operadores
    cout << "[TESTE 4] Validando Heranca Multipla (RelogioCalendario) e operadores ++ / +=...\n";
    RelogioCalendario rc(28, 2, 2024, 23, 58); // 2024 e bissexto
    ++rc;
    assert(rc.getHora() == 23 && rc.getMinuto() == 59);
    ++rc; // Vira para 29/02/2024 00:00
    assert(rc.getDia() == 29 && rc.getMes() == 2 && rc.getAno() == 2024 && rc.getHora() == 0);
    rc += 1440; // +1 dia -> 01/03/2024 00:00
    assert(rc.getDia() == 1 && rc.getMes() == 3 && rc.getAno() == 2024);
    cout << "  -> Virada de ano bissexto e operadores: " << rc << " [SUCESSO]\n\n";

    // Teste 5: Funcoes Template utilitarias
    cout << "[TESTE 5] Validando Funcoes Template (limitarIntervalo, encontrarMaximo)...\n";
    assert(limitarIntervalo(120, 0, 90) == 90);
    assert(limitarIntervalo(-5, 0, 90) == 0);
    assert(quaseIgual(encontrarMaximo(14.4, 22.5), 22.5));
    cout << "  -> Funcoes Template validadas para int e double! [SUCESSO]\n\n";

    // Teste 6: Persistencia e Leitura de Arquivos (<fstream>)
    cout << "[TESTE 6] Validando escrita e leitura de arquivos CSV e TXT (<fstream>)...\n";
    const string arqCsv = "teste_historico_estacionamento.csv";
    const string arqTxt = "teste_relatorio_estacionamento.txt";
    assert(est.exportarHistoricoCSV(arqCsv));
    assert(est.salvarRelatorioTXT(arqTxt));
    int registrosLidos = Estacionamento::carregarContagemRegistrosCSV(arqCsv);
    assert(registrosLidos == 5);
    remove(arqCsv.c_str());
    remove(arqTxt.c_str());
    cout << "  -> Exportacao CSV/TXT e leitura via std::ifstream (" << registrosLidos << " registros) OK! [SUCESSO]\n\n";

    // Teste 7: Desistencia de Veiculo (RECUAR) e Placa Duplicada
    cout << "[TESTE 7] Validando desistencia na cancela e bloqueio de placa duplicada...\n";
    est.chegarVeiculoEntrada(new Carro("DES-7777", "Fiat Argo", 760, false));
    assert(est.getTamanhoFilaEspera() == 1);
    bool recuouOk = est.recuarVeiculoEntrada();
    assert(recuouOk);
    (void)recuouOk;
    assert(est.getTamanhoFilaEspera() == 0);
    assert(est.getEstadoCancela() == "LIVRE");

    est.chegarVeiculoEntrada(new Carro("DUP-8888", "GM Onix", 765, false));
    bool capturouDup = false;
    try {
        est.chegarVeiculoEntrada(new Carro("DUP-8888", "GM Onix", 766, false));
    } catch (const PlacaDuplicadaException& e) {
        capturouDup = true;
        cout << "  -> Excecao de placa duplicada capturada: " << e.what() << endl;
    }
    assert(capturouDup);
    est.emitirTiquete(765);
    est.confirmarTravessiaCancela();
    est.registrarSaida("DUP-8888", 770);
    cout << "  -> Desistencia e prevencao de duplicidade validadas! [SUCESSO]\n\n";

    // Teste 8: Watchdog Concorrente (std::thread + std::mutex + std::atomic)
    cout << "[TESTE 8] Validando Watchdog concorrente (std::thread + std::mutex)...\n";
    est.iniciarWatchdog(120); // Timeout de 120 ms para teste deterministico rapido
    est.chegarVeiculoEntrada(new Carro("WDT-9001", "Ford Ka", 780, false));
    est.emitirTiquete(780);
    assert(est.getEstadoCancela() == "CANCELA_ABERTA");
    // Aguarda thread do watchdog detectar timeout automaticamente
    this_thread::sleep_for(chrono::milliseconds(260));
    assert(est.getEstadoCancela() == "ALERTA_BLOQUEIO");
    assert(est.getHardware()->isAlarmeAtivo());
    assert(est.getDisparosWatchdog() >= 1);
    est.pararWatchdog();

    // Tentar emitir tiquete em ALERTA_BLOQUEIO deve lançar CancelaBloqueadaException
    bool capturouAlerta = false;
    try {
        est.emitirTiquete(785);
    } catch (const CancelaBloqueadaException&) {
        capturouAlerta = true;
    }
    assert(capturouAlerta);

    bool resetOk = est.resetarAlertaCancela();
    assert(resetOk);
    (void)resetOk;
    assert(est.getEstadoCancela() == "LIVRE");
    assert(!est.getHardware()->isAlarmeAtivo());
    est.registrarSaida("WDT-9001", 790);
    cout << "  -> Watchdog multi-thread disparou ALERTA_BLOQUEIO e resetou com seguranca! [SUCESSO]\n\n";

    // Teste 9: Lotacao Maxima e EstacionamentoLotadoException
    cout << "[TESTE 9] Testando condicao de lotacao maxima com excecoes...\n";
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        string placa = "LOT-000" + to_string(i);
        est.chegarVeiculoEntrada(new Carro(placa, "Carro Teste", 800, false));
        est.emitirTiquete(800);
        est.confirmarTravessiaCancela();
    }
    assert(est.getVagasOcupadas() == CAPACIDADE_VAGAS);
    assert(est.getVagasDisponiveis() == 0);

    est.chegarVeiculoEntrada(new Carro("EXC-9999", "Carro Excedente", 800, false));
    bool capturouLotado = false;
    try {
        est.emitirTiquete(800);
    } catch (const EstacionamentoLotadoException& e) {
        capturouLotado = true;
        cout << "  -> Excecao esperada capturada: " << e.what() << endl;
    }
    assert(capturouLotado);
    cout << "  -> Bloqueio por lotacao maxima e excecao validados! [SUCESSO]\n\n";

    // Teste 10: Busca por placa inexistente (TiqueteInvalidoException)
    cout << "[TESTE 10] Testando busca por placa inexistente...\n";
    bool capturouInexistente = false;
    try {
        est.registrarSaida("INEXISTENTE", 900);
    } catch (const TiqueteInvalidoException& e) {
        capturouInexistente = true;
        cout << "  -> Excecao esperada capturada: " << e.what() << endl;
    }
    assert(capturouInexistente);
    cout << "  -> Seguranca contra buscas invalidas validada! [SUCESSO]\n\n";

    est.exibirRelatorioFinanceiro();

    cout << "=================================================================\n";
    cout << "  RESULTADO DO PROJETO INTEGRADO: 10/10 SUITES APROVADAS (100%)  \n";
    cout << "  FSM + TEMPLATE + POLIMORFISMO + HERANCA MULTIPLA + HAL +       \n";
    cout << "  STL + ARQUIVOS + THREADS + EXCECOES + RAII 100% VALIDADOS!     \n";
    cout << "=================================================================\n";
}

void responderJsonStatus(const Estacionamento& est) {
    const IHardwareCancela* hw = est.getHardware();
    cout << "JSON_START {";
    cout << "\"vagas_ocupadas\":" << est.getVagasOcupadas() << ",";
    cout << "\"vagas_disponiveis\":" << est.getVagasDisponiveis() << ",";
    cout << "\"fila_espera\":" << est.getTamanhoFilaEspera() << ",";
    cout << "\"faturamento\":" << fixed << setprecision(2) << est.getFaturamentoTotal() << ",";
    cout << "\"ticket_medio\":" << fixed << setprecision(2) << est.getTicketMedio() << ",";
    cout << "\"total_atendidos\":" << est.getTotalAtendidos() << ",";
    cout << "\"passagens\":" << est.getTotalPassagensCancela() << ",";
    cout << "\"alertas\":" << est.getTotalAlertasCancela() << ",";
    cout << "\"data_hora\":\"" << escaparJson(est.getDataHoraAtual()) << "\",";
    cout << "\"cancela\":\"" << escaparJson(est.getEstadoCancela()) << "\",";
    cout << "\"categorias\":{"
         << "\"Carro\":" << est.getAtendidosCategoria("Carro") << ","
         << "\"Moto\":" << est.getAtendidosCategoria("Moto") << ","
         << "\"Caminhao\":" << est.getAtendidosCategoria("Caminhao")
         << "},";
    cout << "\"hal\":{"
         << "\"servo\":" << (hw ? hw->getAnguloServo() : 0) << ","
         << "\"led_verde\":" << ((hw && hw->isLedVerdeAceso()) ? "true" : "false") << ","
         << "\"alarme\":" << ((hw && hw->isAlarmeAtivo()) ? "true" : "false") << ","
         << "\"lcd1\":\"" << escaparJson(hw ? hw->getDisplayLinha1() : "") << "\","
         << "\"lcd2\":\"" << escaparJson(hw ? hw->getDisplayLinha2() : "") << "\""
         << "},";
    cout << "\"vagas\":[";
    for (int i = 0; i < est.getCapacidade(); ++i) {
        const Veiculo* v = est.getVeiculoVaga(i);
        if (i > 0) cout << ",";
        if (v == nullptr) {
            cout << "{\"vaga\":" << (i + 1) << ",\"ocupada\":false}";
        } else {
            cout << "{\"vaga\":" << (i + 1) << ",\"ocupada\":true"
                 << ",\"tipo\":\"" << escaparJson(v->getTipo()) << "\""
                 << ",\"placa\":\"" << escaparJson(v->getPlaca()) << "\""
                 << ",\"modelo\":\"" << escaparJson(v->getModelo()) << "\""
                 << ",\"entrada\":" << v->getMinutoEntrada() << "}";
        }
    }
    cout << "]} JSON_END\n";
    cout.flush();
}

void executarModoPipe() {
    Estacionamento est;
    string linha;

    cout << "SMART_PARKING_ENGINE_ONLINE\n";
    cout.flush();

    while (getline(cin, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        string cmd;
        ss >> cmd;

        if (cmd == "STATUS") {
            responderJsonStatus(est);
        } else if (cmd == "CHEGAR") {
            // Formato: CHEGAR <tipo> <placa> <modelo (pode conter espacos)> <minuto> <extra>
            vector<string> tokens;
            string tok;
            while (ss >> tok) {
                tokens.push_back(tok);
            }
            if (tokens.size() < 5) {
                cout << "ERRO Argumentos insuficientes para comando CHEGAR.\n";
                cout.flush();
                continue;
            }
            string tipo = tokens[0];
            string placa = tokens[1];
            string modelo;
            for (size_t i = 2; i + 2 < tokens.size(); ++i) {
                if (!modelo.empty()) modelo += " ";
                modelo += tokens[i];
            }
            int minuto = 0;
            int extra = 0;
            try {
                minuto = stoi(tokens[tokens.size() - 2]);
                extra = stoi(tokens[tokens.size() - 1]);
            } catch (const exception&) {
                cout << "ERRO Parametros numericos invalidos no comando CHEGAR.\n";
                cout.flush();
                continue;
            }

            Veiculo* v = nullptr;
            if (tipo == "Carro") {
                v = new Carro(placa, modelo, minuto, (extra == 1));
            } else if (tipo == "Moto") {
                v = new Moto(placa, modelo, minuto, extra);
            } else if (tipo == "Caminhao") {
                v = new Caminhao(placa, modelo, minuto, extra);
            }

            if (v) {
                try {
                    est.chegarVeiculoEntrada(v);
                    cout << "OK Veiculo " << placa << " (" << tipo << " " << modelo << ") chegou a cancela.\n";
                } catch (const exception& e) {
                    cout << "ERRO " << e.what() << "\n";
                }
            } else {
                cout << "ERRO Tipo de veiculo desconhecido.\n";
            }
            cout.flush();
        } else if (cmd == "EMITIR") {
            int minuto = est.getMinutoAtualSistema();
            ss >> minuto;
            try {
                Tiquete t = est.emitirTiquete(minuto);
                cout << "OK TIQUETE " << t.getId() << " " << t.getPlaca() << " "
                     << t.getTipoVeiculo() << " Vaga " << t.getNumeroVaga() << "\n";
            } catch (const exception& e) {
                cout << "ERRO " << e.what() << "\n";
            }
            cout.flush();
        } else if (cmd == "PASSAR") {
            bool ok = est.confirmarTravessiaCancela();
            if (ok) {
                cout << "OK Travessia confirmada. Cancela fechada.\n";
            } else {
                cout << "ERRO Falha ao confirmar travessia (estado atual: " << est.getEstadoCancela() << ").\n";
            }
            cout.flush();
        } else if (cmd == "RECUAR") {
            bool ok = est.recuarVeiculoEntrada();
            if (ok) {
                cout << "OK Veiculo recuou da cancela com seguranca.\n";
            } else {
                cout << "ERRO Nao ha veiculo em posicao de desistencia.\n";
            }
            cout.flush();
        } else if (cmd == "TIMEOUT") {
            bool ok = est.simularTimeoutCancela();
            if (ok) {
                cout << "OK ALERTA_BLOQUEIO acionado por timeout de passagem!\n";
            } else {
                cout << "ERRO Timeout so ocorre com a cancela aberta.\n";
            }
            cout.flush();
        } else if (cmd == "RESET") {
            bool ok = est.resetarAlertaCancela();
            if (ok) {
                cout << "OK Alerta desarmado e sistema restaurado.\n";
            } else {
                cout << "ERRO Sistema nao esta em estado de alerta.\n";
            }
            cout.flush();
        } else if (cmd == "RELOGIO") {
            int minuto = 0;
            if (ss >> minuto) {
                est.sincronizarRelogio(minuto);
                cout << "OK Relogio sincronizado: " << est.getDataHoraAtual() << "\n";
            } else {
                cout << "ERRO Informe o minuto para RELOGIO.\n";
            }
            cout.flush();
        } else if (cmd == "EXPORTAR") {
            string prefixo = "relatorio_smart_parking";
            ss >> prefixo;
            bool okCsv = est.exportarHistoricoCSV(prefixo + ".csv");
            bool okTxt = est.salvarRelatorioTXT(prefixo + ".txt");
            if (okCsv && okTxt) {
                cout << "OK Arquivos exportados: " << prefixo << ".csv e " << prefixo << ".txt\n";
            } else {
                cout << "ERRO Falha ao gravar arquivos de relatorio.\n";
            }
            cout.flush();
        } else if (cmd == "SAIR") {
            string placa;
            int minutoSaida = est.getMinutoAtualSistema();
            ss >> placa >> minutoSaida;
            try {
                double valor = est.registrarSaida(placa, minutoSaida);
                cout << "OK SAIDA " << placa << " R$ " << fixed << setprecision(2) << valor << "\n";
            } catch (const exception& e) {
                cout << "ERRO " << e.what() << "\n";
            }
            cout.flush();
        } else if (cmd == "FIM" || cmd == "SAIR_APP") {
            cout << "BYE\n";
            cout.flush();
            break;
        } else {
            cout << "ERRO Comando desconhecido: " << cmd << "\n";
            cout.flush();
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--pipe") {
        executarModoPipe();
    } else {
        executarSuiteTestesAutomatizados();
    }
    return 0;
}
