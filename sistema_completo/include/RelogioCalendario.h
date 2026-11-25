#ifndef RELOGIO_CALENDARIO_H
#define RELOGIO_CALENDARIO_H

#include <iostream>
#include <string>

/**
 * @brief Classe base Relogio para controle de hora e minuto.
 * Modelada segundo o padrão clássico de Herança Múltipla da disciplina EEL7323.
 */
class Relogio {
protected:
    int hora;
    int minuto;

public:
    Relogio(int h = 8, int m = 0);
    virtual ~Relogio() = default;

    int getHora() const { return hora; }
    int getMinuto() const { return minuto; }
    int getMinutosTotaisDia() const { return hora * 60 + minuto; }

    /**
     * @brief Avança o relógio em uma quantidade de minutos.
     * @return Número de dias completos ultrapassados (viradas de meia-noite).
     */
    int avancarMinutosRelogio(int deltaMinutos);
};

/**
 * @brief Classe base Calendario para controle de dia, mês e ano.
 */
class Calendario {
protected:
    int dia;
    int mes;
    int ano;

    static bool isAnoBissexto(int a);
    static int diasNoMes(int m, int a);

public:
    Calendario(int d = 1, int m = 1, int a = 2026);
    virtual ~Calendario() = default;

    int getDia() const { return dia; }
    int getMes() const { return mes; }
    int getAno() const { return ano; }

    void avancarDias(int dias);
};

/**
 * @brief Classe derivada via Herança Múltipla combinando Relogio e Calendario.
 *
 * Demonstra:
 * - Herança múltipla (Relogio + Calendario);
 * - Sobrecarga de operadores (++ pré-fixado, +=, ==) e função friend (operator<<);
 * - Propagação determinística de virada de dia para carimbo temporal de tíquetes e logs.
 */
class RelogioCalendario : public Relogio, public Calendario {
private:
    int minutosAcumulados; // Minutos contínuos desde o início da operação do sistema

public:
    RelogioCalendario(int d = 24, int m = 11, int a = 2026, int h = 8, int min = 0);
    virtual ~RelogioCalendario() = default;

    int getMinutosAcumulados() const { return minutosAcumulados; }
    void sincronizarMinutoDia(int minutoAbsoluto);

    std::string formatarDataHora() const;
    std::string formatarHora() const;

    // Sobrecarga de operadores
    RelogioCalendario& operator++();                 // Avança 1 minuto (++relogio)
    RelogioCalendario& operator+=(int deltaMinutos); // Avança N minutos
    bool operator==(const RelogioCalendario& outro) const;

    friend std::ostream& operator<<(std::ostream& os, const RelogioCalendario& rc);
};

#endif // RELOGIO_CALENDARIO_H
