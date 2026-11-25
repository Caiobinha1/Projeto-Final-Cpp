#ifndef FSM_CONTROLADOR_H
#define FSM_CONTROLADOR_H

#include <string>
#include <iostream>

/**
 * @brief Enumeração dos estados operacionais da cancela e do controlador de acesso.
 */
enum EstadoControlador {
    ESTADO_LIVRE = 0,            ///< Cancela fechada, nenhum veículo no laço indutivo
    ESTADO_VEICULO_DETECTADO,    ///< Veículo sobre o laço indutivo de entrada
    ESTADO_EMITINDO_TIQUETE,     ///< Botão pressionado, tíquete sendo impresso
    ESTADO_CANCELA_ABERTA,       ///< Tíquete retirado, atuador abre a cancela
    ESTADO_PASSAGEM_CONCLUIDA,   ///< Sensor de passagem confirma saída do veículo do vão
    ESTADO_ALERTA_BLOQUEIO       ///< Falha de tempo (timeout) ou evento irregular
};

/**
 * @brief Enumeração dos eventos que disparam transições na Máquina de Estados.
 */
enum EventoControlador {
    EVENTO_VEICULO_CHEGOU = 0,   ///< Sensor de presença detecta veículo
    EVENTO_BOTAO_PRESSIONADO,    ///< Condutor solicita tíquete no totem
    EVENTO_TIQUETE_RETIRADO,     ///< Condutor retira o tíquete impresso
    EVENTO_SENSOR_PASSAGEM,      ///< Sensor óptico detecta travessia completa
    EVENTO_TIMEOUT_PASSAGEM,     ///< Tempo limite para cruzar a cancela expirou
    EVENTO_RESET_SISTEMA,        ///< Comando manual de desobstrução e reinicialização
    EVENTO_VEICULO_RECUOU        ///< Veículo desistiu antes de retirar tíquete
};

/**
 * @brief Classe que implementa a Máquina de Estados Finitos (FSM) do controlador.
 * 
 * Segue os princípios de encapsulamento da disciplina EEL7323: estado interno privado,
 * métodos de consulta const e transição estritamente determinística.
 */
class FSM_Controlador {
private:
    EstadoControlador estadoAtual;
    int totalPassagensSucesso;
    int totalAlertas;

public:
    FSM_Controlador();
    ~FSM_Controlador() = default;

    /**
     * @brief Processa um evento de entrada e executa a transição de estado.
     * @param evento Evento ocorrido no sistema físico.
     * @return true se a transição foi válida; false se o evento foi ignorado/inválido.
     */
    bool processarEvento(EventoControlador evento);

    // Métodos de consulta (const)
    EstadoControlador getEstadoAtual() const { return estadoAtual; }
    std::string getNomeEstadoAtual() const;
    static std::string getNomeEstado(EstadoControlador estado);
    static std::string getNomeEvento(EventoControlador evento);

    bool isCancelaAberta() const { return estadoAtual == ESTADO_CANCELA_ABERTA; }
    bool isEmAlerta() const { return estadoAtual == ESTADO_ALERTA_BLOQUEIO; }

    int getTotalPassagensSucesso() const { return totalPassagensSucesso; }
    int getTotalAlertas() const { return totalAlertas; }

    void reset();
};

#endif // FSM_CONTROLADOR_H
