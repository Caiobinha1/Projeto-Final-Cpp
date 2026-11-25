#include <iostream>
#include <cassert>
#include "../include/FSM_Controlador.h"

using namespace std;

void imprimirPasso(const FSM_Controlador& fsm, EventoControlador evt, bool sucesso) {
    cout << "  [Evento: " << FSM_Controlador::getNomeEvento(evt) << "] -> "
         << (sucesso ? "Transição OK" : "Transição REJEITADA")
         << " | Estado Atual: " << fsm.getNomeEstadoAtual() << endl;
}

int main() {
    cout << "========================================================\n";
    cout << "  UFSC / CTC / EEL - EEL7323 Programacao C++ Embarcada  \n";
    cout << "  PROJETO FINAL - SMART PARKING CONTROLLER              \n";
    cout << "  SUITE DE TESTES: MODULO 1 (MAQUINA DE ESTADOS - FSM)  \n";
    cout << "========================================================\n\n";

    FSM_Controlador fsm;

    // Teste 1: Estado Inicial
    cout << "[TESTE 1] Verificando estado inicial do controlador...\n";
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    assert(!fsm.isCancelaAberta());
    assert(!fsm.isEmAlerta());
    cout << "  -> Estado inicial verificado: " << fsm.getNomeEstadoAtual() << " [SUCESSO]\n\n";

    // Teste 2: Ciclo de Passagem Completo com Sucesso
    cout << "[TESTE 2] Simulando ciclo completo de passagem normal...\n";
    bool ok = fsm.processarEvento(EVENTO_VEICULO_CHEGOU);
    imprimirPasso(fsm, EVENTO_VEICULO_CHEGOU, ok);
    assert(fsm.getEstadoAtual() == ESTADO_VEICULO_DETECTADO);

    ok = fsm.processarEvento(EVENTO_BOTAO_PRESSIONADO);
    imprimirPasso(fsm, EVENTO_BOTAO_PRESSIONADO, ok);
    assert(fsm.getEstadoAtual() == ESTADO_EMITINDO_TIQUETE);

    ok = fsm.processarEvento(EVENTO_TIQUETE_RETIRADO);
    imprimirPasso(fsm, EVENTO_TIQUETE_RETIRADO, ok);
    assert(fsm.getEstadoAtual() == ESTADO_CANCELA_ABERTA);
    assert(fsm.isCancelaAberta());

    ok = fsm.processarEvento(EVENTO_SENSOR_PASSAGEM);
    imprimirPasso(fsm, EVENTO_SENSOR_PASSAGEM, ok);
    assert(fsm.getEstadoAtual() == ESTADO_PASSAGEM_CONCLUIDA);

    // Conclusão automática para LIVRE
    ok = fsm.processarEvento(EVENTO_SENSOR_PASSAGEM); // Qualquer pulso de fechamento
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    assert(fsm.getTotalPassagensSucesso() == 1);
    cout << "  -> Ciclo normal finalizado com sucesso! [SUCESSO]\n\n";

    // Teste 3: Rejeição de Eventos Inválidos
    cout << "[TESTE 3] Testando robustez contra transicoes invalidas...\n";
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    // Tentar retirar tíquete ou abrir cancela sem veículo presente
    ok = fsm.processarEvento(EVENTO_TIQUETE_RETIRADO);
    imprimirPasso(fsm, EVENTO_TIQUETE_RETIRADO, ok);
    assert(!ok);
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);

    ok = fsm.processarEvento(EVENTO_SENSOR_PASSAGEM);
    imprimirPasso(fsm, EVENTO_SENSOR_PASSAGEM, ok);
    assert(!ok);
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    cout << "  -> Eventos ilegais foram bloqueados corretamente! [SUCESSO]\n\n";

    // Teste 4: Ciclo com Timeout e Modo de Alerta
    cout << "[TESTE 4] Simulando timeout de passagem e recuperacao de alerta...\n";
    fsm.processarEvento(EVENTO_VEICULO_CHEGOU);
    fsm.processarEvento(EVENTO_BOTAO_PRESSIONADO);
    fsm.processarEvento(EVENTO_TIQUETE_RETIRADO);
    assert(fsm.isCancelaAberta());

    // Veículo não passou e expirou o tempo
    ok = fsm.processarEvento(EVENTO_TIMEOUT_PASSAGEM);
    imprimirPasso(fsm, EVENTO_TIMEOUT_PASSAGEM, ok);
    assert(fsm.isEmAlerta());
    assert(fsm.getTotalAlertas() == 1);

    // Em alerta, não aceita novos veículos
    ok = fsm.processarEvento(EVENTO_VEICULO_CHEGOU);
    imprimirPasso(fsm, EVENTO_VEICULO_CHEGOU, ok);
    assert(!ok);

    // Reset manual do operador
    ok = fsm.processarEvento(EVENTO_RESET_SISTEMA);
    imprimirPasso(fsm, EVENTO_RESET_SISTEMA, ok);
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    cout << "  -> Tratamento de timeout e alerta validado! [SUCESSO]\n\n";

    // Teste 5: Desistência do Veículo
    cout << "[TESTE 5] Simulando desistencia do condutor (veiculo recuou)...\n";
    fsm.processarEvento(EVENTO_VEICULO_CHEGOU);
    assert(fsm.getEstadoAtual() == ESTADO_VEICULO_DETECTADO);
    ok = fsm.processarEvento(EVENTO_VEICULO_RECUOU);
    imprimirPasso(fsm, EVENTO_VEICULO_RECUOU, ok);
    assert(fsm.getEstadoAtual() == ESTADO_LIVRE);
    cout << "  -> Desistencia tratada com retorno ao estado LIVRE! [SUCESSO]\n\n";

    cout << "========================================================\n";
    cout << "  RESULTADO FINAL MODULO 1: 100% DOS TESTES PASSARAM!   \n";
    cout << "  Total de passagens com sucesso: " << fsm.getTotalPassagensSucesso() << "\n";
    cout << "  Total de alertas gerenciados: " << fsm.getTotalAlertas() << "\n";
    cout << "========================================================\n";

    return 0;
}
