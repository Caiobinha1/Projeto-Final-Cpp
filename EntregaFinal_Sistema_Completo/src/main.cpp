#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <iomanip>
#include "../include/Estacionamento.h"
#include "../include/Carro.h"
#include "../include/Moto.h"
#include "../include/Caminhao.h"
#include "../include/Excecoes.h"

using namespace std;

void executarSuiteTestesAutomatizados() {
    cout << "=================================================================\n";
    cout << "  UFSC / CTC / EEL - EEL7323 Programacao C++ para Embarcados     \n";
    cout << "  PROJETO FINAL INTEGRADO - SMART PARKING CONTROLLER             \n";
    cout << "  BATERIA COMPLETA DE TESTES AUTOMATIZADOS (SUITE DE INTEGRACAO) \n";
    cout << "=================================================================\n\n";

    Estacionamento est;

    // Teste 1: Estado Inicial
    cout << "[TESTE 1] Verificando condicoes iniciais do estacionamento...\n";
    assert(est.getVagasDisponiveis() == CAPACIDADE_VAGAS);
    assert(est.getVagasOcupadas() == 0);
    assert(est.getTamanhoFilaEspera() == 0);
    assert(est.getFaturamentoTotal() == 0.0);
    assert(est.getTotalAtendidos() == 0);
    cout << "  -> Patio vazio e inicializado com sucesso (" << CAPACIDADE_VAGAS << " vagas livres). [SUCESSO]\n\n";

    // Teste 2: Entrada e Emissão de Tíquetes para Múltiplas Categorias
    cout << "[TESTE 2] Simulando chegada e emissao de tiquetes para 3 categorias...\n";
    est.chegarVeiculoEntrada(new Carro("CAR-1001", "VW Polo", 480, false));
    assert(est.getTamanhoFilaEspera() == 1);
    Tiquete t1 = est.emitirTiquete(480);
    cout << "  Emitido: " << t1 << endl;
    assert(t1.getPlaca() == "CAR-1001");
    assert(t1.getTipoVeiculo() == "Carro");
    assert(t1.getNumeroVaga() == 1);
    assert(est.confirmarTravessiaCancela());

    est.chegarVeiculoEntrada(new Moto("MOT-2002", "Honda CB500", 495, 500));
    Tiquete t2 = est.emitirTiquete(495);
    cout << "  Emitido: " << t2 << endl;
    assert(t2.getPlaca() == "MOT-2002");
    assert(t2.getNumeroVaga() == 2);
    assert(est.confirmarTravessiaCancela());

    est.chegarVeiculoEntrada(new Caminhao("CAM-3003", "Volvo FH", 510, 3));
    Tiquete t3 = est.emitirTiquete(510);
    cout << "  Emitido: " << t3 << endl;
    assert(t3.getPlaca() == "CAM-3003");
    assert(t3.getNumeroVaga() == 3);
    assert(est.confirmarTravessiaCancela());

    assert(est.getVagasOcupadas() == 4);
    assert(est.getVagasDisponiveis() == 6);
    cout << "  -> 3 categorias alocadas com alocacao fisica de vagas correta! [SUCESSO]\n\n";

    est.exibirStatus();

    // Teste 3: Processamento de Saídas e Cálculo Tarifário Polimórfico
    cout << "\n[TESTE 3] Processando saidas e validando calculo tarifario polimorfico...\n";
    double valorCarro = est.registrarSaida("CAR-1001", 630);
    cout << "  Saida Carro CAR-1001 (150 min): R$ " << fixed << setprecision(2) << valorCarro << endl;
    assert(valorCarro == 16.0);

    double valorMoto = est.registrarSaida("MOT-2002", 505);
    cout << "  Saida Moto MOT-2002 (10 min - Tolerancia): R$ " << valorMoto << endl;
    assert(valorMoto == 0.0);

    double valorCaminhao = est.registrarSaida("CAM-3003", 600);
    cout << "  Saida Caminhao CAM-3003 (90 min, 3 eixos): R$ " << valorCaminhao << endl;
    assert(valorCaminhao == 22.50);

    assert(est.getVagasOcupadas() == 0);
    assert(est.getVagasDisponiveis() == CAPACIDADE_VAGAS);
    assert(est.getTotalAtendidos() == 3);
    assert(est.getFaturamentoTotal() == (16.0 + 0.0 + 22.50));
    cout << "  -> Tarifacao polimorfica e desocupacao fisica de vagas validadas! [SUCESSO]\n\n";

    // Teste 4: Teste de Lotação e Captura de EstacionamentoLotadoException
    cout << "[TESTE 4] Testando condicao de lotacao maxima com excecoes...\n";
    for (int i = 0; i < CAPACIDADE_VAGAS; ++i) {
        string placa = "LOT-000" + to_string(i);
        est.chegarVeiculoEntrada(new Carro(placa, "Carro Teste", 600, false));
        est.emitirTiquete(600);
        est.confirmarTravessiaCancela();
    }
    assert(est.getVagasOcupadas() == CAPACIDADE_VAGAS);
    assert(est.getVagasDisponiveis() == 0);

    est.chegarVeiculoEntrada(new Carro("EXC-9999", "Carro Excedente", 600, false));
    bool capturouLotado = false;
    try {
        est.emitirTiquete(600);
    } catch (const EstacionamentoLotadoException& e) {
        capturouLotado = true;
        cout << "  -> Excecao esperada capturada: " << e.what() << endl;
    }
    assert(capturouLotado);
    cout << "  -> Bloqueio por lotacao maxima e excecao validados! [SUCESSO]\n\n";

    // Teste 5: Tentativa de saída de veículo inexistente
    cout << "[TESTE 5] Testando busca por placa inexistente...\n";
    bool capturouInexistente = false;
    try {
        est.registrarSaida("INEXISTENTE", 700);
    } catch (const TiqueteInvalidoException& e) {
        capturouInexistente = true;
        cout << "  -> Excecao esperada capturada: " << e.what() << endl;
    }
    assert(capturouInexistente);
    cout << "  -> Seguranca contra buscas invalidas validada! [SUCESSO]\n\n";

    est.exibirRelatorioFinanceiro();

    cout << "=================================================================\n";
    cout << "  RESULTADO DO PROJETO INTEGRADO: 100% DOS TESTES APROVADOS!     \n";
    cout << "  FSM + FILA TEMPLATE + POLIMORFISMO + EXCECOES + RAII 100% OK!  \n";
    cout << "=================================================================\n";
}

void responderJsonStatus(const Estacionamento& est) {
    cout << "JSON_START {";
    cout << "\"vagas_ocupadas\":" << est.getVagasOcupadas() << ",";
    cout << "\"vagas_disponiveis\":" << est.getVagasDisponiveis() << ",";
    cout << "\"fila_espera\":" << est.getTamanhoFilaEspera() << ",";
    cout << "\"faturamento\":" << fixed << setprecision(2) << est.getFaturamentoTotal() << ",";
    cout << "\"total_atendidos\":" << est.getTotalAtendidos() << ",";
    cout << "\"cancela\":\"" << est.getEstadoCancela() << "\",";
    cout << "\"vagas\":[";
    for (int i = 0; i < est.getCapacidade(); ++i) {
        const Veiculo* v = est.getVeiculoVaga(i);
        if (i > 0) cout << ",";
        if (v == nullptr) {
            cout << "{\"vaga\":" << (i + 1) << ",\"ocupada\":false}";
        } else {
            cout << "{\"vaga\":" << (i + 1) << ",\"ocupada\":true"
                 << ",\"tipo\":\"" << v->getTipo() << "\""
                 << ",\"placa\":\"" << v->getPlaca() << "\""
                 << ",\"modelo\":\"" << v->getModelo() << "\""
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
            string tipo, placa, modelo;
            int minuto, extra;
            ss >> tipo >> placa >> modelo >> minuto >> extra;
            Veiculo* v = nullptr;
            if (tipo == "Carro") {
                v = new Carro(placa, modelo, minuto, (extra == 1));
            } else if (tipo == "Moto") {
                v = new Moto(placa, modelo, minuto, extra);
            } else if (tipo == "Caminhao") {
                v = new Caminhao(placa, modelo, minuto, extra);
            }
            if (v) {
                est.chegarVeiculoEntrada(v);
                cout << "OK Veiculo " << placa << " (" << tipo << ") chegou a cancela.\n";
            } else {
                cout << "ERRO Tipo de veiculo desconhecido.\n";
            }
            cout.flush();
        } else if (cmd == "EMITIR") {
            int minuto;
            ss >> minuto;
            try {
                Tiquete t = est.emitirTiquete(minuto);
                cout << "OK TIQUETE " << t.getId() << " " << t.getPlaca() << " " << t.getTipoVeiculo() << " Vaga " << t.getNumeroVaga() << "\n";
            } catch (const exception& e) {
                cout << "ERRO " << e.what() << "\n";
            }
            cout.flush();
        } else if (cmd == "PASSAR") {
            bool ok = est.confirmarTravessiaCancela();
            if (ok) {
                cout << "OK Travessia confirmada. Cancela fechada.\n";
            } else {
                cout << "ERRO Falha ao confirmar travessia.\n";
            }
            cout.flush();
        } else if (cmd == "SAIR") {
            string placa;
            int minutoSaida;
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
