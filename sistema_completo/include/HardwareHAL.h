#ifndef HARDWARE_HAL_H
#define HARDWARE_HAL_H

#include <string>
#include "UtilitariosTemplate.h"

/**
 * @brief Interface abstrata de Camada de Abstração de Hardware (HAL).
 *
 * Inspirada no padrão Robot/RobotLinux da disciplina EEL7323: desacopla a
 * lógica de controle (Estacionamento / FSM_Controlador) dos periféricos físicos
 * (servo motor SG90, LEDs semafóricos, buzzer e display LCD 16x2).
 * Permite executar o mesmo núcleo C++ tanto em simulação no PC quanto embarcado
 * em Arduino, ESP32-C3, STM32F429 ou FPGA Atlys/LEON3.
 */
class IHardwareCancela {
public:
    virtual ~IHardwareCancela() = default;

    virtual void acionarMotorCancela(bool abrir) = 0;
    virtual void acionarSemaforo(bool verde) = 0;
    virtual void acionarAlarmeSonoro(bool ativo) = 0;
    virtual void atualizarDisplayLCD(const std::string& linha1, const std::string& linha2) = 0;

    virtual int getAnguloServo() const = 0;
    virtual bool isLedVerdeAceso() const = 0;
    virtual bool isAlarmeAtivo() const = 0;
    virtual std::string getDisplayLinha1() const = 0;
    virtual std::string getDisplayLinha2() const = 0;
    virtual std::string getNomePlataforma() const = 0;
};

/**
 * @brief Implementação simulada dos periféricos embarcados em memória (Host/PC).
 *
 * Mantém o estado completo dos atuadores e do display LCD 16x2 para verificação
 * em testes automatizados e visualização em tempo real no Simulador Visual.
 */
class HardwareSimulado : public IHardwareCancela {
private:
    int anguloServo;      // 0 graus (fechada) a 90 graus (aberta)
    bool ledVerde;        // true = Verde (Siga), false = Vermelho (Pare)
    bool alarmeSonoro;    // true = Buzzer acionado (Alerta/Timeout)
    std::string lcdLinha1;
    std::string lcdLinha2;

    static std::string ajustarLinhaLCD(const std::string& texto) {
        if (texto.size() > 16) {
            return texto.substr(0, 16);
        }
        return texto;
    }

public:
    HardwareSimulado()
        : anguloServo(0),
          ledVerde(false),
          alarmeSonoro(false),
          lcdLinha1("CANCELA PRONTA"),
          lcdLinha2("VAGAS: 10 LIVRES") {}

    virtual ~HardwareSimulado() override = default;

    virtual void acionarMotorCancela(bool abrir) override {
        anguloServo = limitarIntervalo(abrir ? 90 : 0, 0, 90);
    }

    virtual void acionarSemaforo(bool verde) override {
        ledVerde = verde;
    }

    virtual void acionarAlarmeSonoro(bool ativo) override {
        alarmeSonoro = ativo;
    }

    virtual void atualizarDisplayLCD(const std::string& linha1, const std::string& linha2) override {
        lcdLinha1 = ajustarLinhaLCD(linha1);
        lcdLinha2 = ajustarLinhaLCD(linha2);
    }

    virtual int getAnguloServo() const override { return anguloServo; }
    virtual bool isLedVerdeAceso() const override { return ledVerde; }
    virtual bool isAlarmeAtivo() const override { return alarmeSonoro; }
    virtual std::string getDisplayLinha1() const override { return lcdLinha1; }
    virtual std::string getDisplayLinha2() const override { return lcdLinha2; }
    virtual std::string getNomePlataforma() const override { return "HAL_Simulada_Host_PC"; }
};

#endif // HARDWARE_HAL_H
