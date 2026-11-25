#include "../include/FSM_Controlador.h"

FSM_Controlador::FSM_Controlador()
    : estadoAtual(ESTADO_LIVRE), totalPassagensSucesso(0), totalAlertas(0) {}

bool FSM_Controlador::processarEvento(EventoControlador evento) {
    bool transicaoValida = false;

    switch (estadoAtual) {
        case ESTADO_LIVRE:
            if (evento == EVENTO_VEICULO_CHEGOU) {
                estadoAtual = ESTADO_VEICULO_DETECTADO;
                transicaoValida = true;
            }
            break;

        case ESTADO_VEICULO_DETECTADO:
            if (evento == EVENTO_BOTAO_PRESSIONADO) {
                estadoAtual = ESTADO_EMITINDO_TIQUETE;
                transicaoValida = true;
            } else if (evento == EVENTO_VEICULO_RECUOU) {
                estadoAtual = ESTADO_LIVRE;
                transicaoValida = true;
            }
            break;

        case ESTADO_EMITINDO_TIQUETE:
            if (evento == EVENTO_TIQUETE_RETIRADO) {
                estadoAtual = ESTADO_CANCELA_ABERTA;
                transicaoValida = true;
            }
            break;

        case ESTADO_CANCELA_ABERTA:
            if (evento == EVENTO_SENSOR_PASSAGEM) {
                estadoAtual = ESTADO_PASSAGEM_CONCLUIDA;
                totalPassagensSucesso++;
                transicaoValida = true;
            } else if (evento == EVENTO_TIMEOUT_PASSAGEM) {
                estadoAtual = ESTADO_ALERTA_BLOQUEIO;
                totalAlertas++;
                transicaoValida = true;
            }
            break;

        case ESTADO_PASSAGEM_CONCLUIDA:
            // Transição automática para LIVRE assim que a cancela baixa com segurança
            estadoAtual = ESTADO_LIVRE;
            transicaoValida = true;
            break;

        case ESTADO_ALERTA_BLOQUEIO:
            if (evento == EVENTO_RESET_SISTEMA) {
                estadoAtual = ESTADO_LIVRE;
                transicaoValida = true;
            }
            break;

        default:
            transicaoValida = false;
            break;
    }

    return transicaoValida;
}

std::string FSM_Controlador::getNomeEstadoAtual() const {
    return getNomeEstado(estadoAtual);
}

std::string FSM_Controlador::getNomeEstado(EstadoControlador estado) {
    switch (estado) {
        case ESTADO_LIVRE: return "LIVRE";
        case ESTADO_VEICULO_DETECTADO: return "VEICULO_DETECTADO";
        case ESTADO_EMITINDO_TIQUETE: return "EMITINDO_TIQUETE";
        case ESTADO_CANCELA_ABERTA: return "CANCELA_ABERTA";
        case ESTADO_PASSAGEM_CONCLUIDA: return "PASSAGEM_CONCLUIDA";
        case ESTADO_ALERTA_BLOQUEIO: return "ALERTA_BLOQUEIO";
        default: return "DESCONHECIDO";
    }
}

std::string FSM_Controlador::getNomeEvento(EventoControlador evento) {
    switch (evento) {
        case EVENTO_VEICULO_CHEGOU: return "VEICULO_CHEGOU";
        case EVENTO_BOTAO_PRESSIONADO: return "BOTAO_PRESSIONADO";
        case EVENTO_TIQUETE_RETIRADO: return "TIQUETE_RETIRADO";
        case EVENTO_SENSOR_PASSAGEM: return "SENSOR_PASSAGEM";
        case EVENTO_TIMEOUT_PASSAGEM: return "TIMEOUT_PASSAGEM";
        case EVENTO_RESET_SISTEMA: return "RESET_SISTEMA";
        case EVENTO_VEICULO_RECUOU: return "VEICULO_RECUOU";
        default: return "EVENTO_DESCONHECIDO";
    }
}

void FSM_Controlador::reset() {
    estadoAtual = ESTADO_LIVRE;
}
