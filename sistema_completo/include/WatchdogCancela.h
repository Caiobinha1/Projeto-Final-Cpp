#ifndef WATCHDOG_CANCELA_H
#define WATCHDOG_CANCELA_H

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include "FSM_Controlador.h"

/**
 * @brief Monitor concorrente (Watchdog Timer) para segurança da cancela (Semana 11 - Threads).
 *
 * Executa em uma thread dedicada (std::thread) protegida por std::mutex e controlada
 * por std::atomic<bool>. Caso a FSM permaneça em ESTADO_CANCELA_ABERTA por mais tempo
 * que timeoutMs, o watchdog dispara automaticamente EVENTO_TIMEOUT_PASSAGEM e invoca
 * o callback de alerta (acionando buzzer/LCD na HAL).
 */
class WatchdogCancela {
private:
    std::atomic<bool> executando;
    std::atomic<int> disparosTimeout;
    std::thread worker;
    std::mutex* mutexEstado;
    FSM_Controlador* fsm;
    int timeoutMs;
    int intervaloVerificacaoMs;
    std::function<void()> callbackAlerta;

    void loopMonitoramento() {
        bool cronometroAtivo = false;
        std::chrono::steady_clock::time_point inicioAbertura;

        while (executando.load()) {
            bool dispararCallback = false;

            {
                std::lock_guard<std::mutex> lock(*mutexEstado);
                if (fsm->isCancelaAberta()) {
                    if (!cronometroAtivo) {
                        cronometroAtivo = true;
                        inicioAbertura = std::chrono::steady_clock::now();
                    } else {
                        auto decorrido = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - inicioAbertura
                        ).count();

                        if (decorrido >= timeoutMs) {
                            if (fsm->processarEvento(EVENTO_TIMEOUT_PASSAGEM)) {
                                disparosTimeout++;
                                cronometroAtivo = false;
                                dispararCallback = true;
                            }
                        }
                    }
                } else {
                    cronometroAtivo = false;
                }
            }

            if (dispararCallback && callbackAlerta) {
                callbackAlerta();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(intervaloVerificacaoMs));
        }
    }

public:
    WatchdogCancela(FSM_Controlador* controlador,
                    std::mutex* mtx,
                    int limiteMs = 10000,
                    int passoMs = 20,
                    std::function<void()> onAlerta = nullptr)
        : executando(false),
          disparosTimeout(0),
          mutexEstado(controlador ? mtx : nullptr),
          fsm(controlador),
          timeoutMs(limiteMs),
          intervaloVerificacaoMs(passoMs),
          callbackAlerta(onAlerta) {}

    ~WatchdogCancela() {
        parar();
    }

    WatchdogCancela(const WatchdogCancela&) = delete;
    WatchdogCancela& operator=(const WatchdogCancela&) = delete;

    void iniciar() {
        if (executando.load() || fsm == nullptr || mutexEstado == nullptr) return;
        executando.store(true);
        worker = std::thread(&WatchdogCancela::loopMonitoramento, this);
    }

    void parar() {
        if (executando.exchange(false)) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    bool isAtivo() const { return executando.load(); }
    int getDisparosTimeout() const { return disparosTimeout.load(); }
};

#endif // WATCHDOG_CANCELA_H
