#include "../include/RelogioCalendario.h"
#include <iomanip>
#include <sstream>

Relogio::Relogio(int h, int m) : hora(h), minuto(m) {
    if (hora < 0 || hora > 23) hora = 0;
    if (minuto < 0 || minuto > 59) minuto = 0;
}

int Relogio::avancarMinutosRelogio(int deltaMinutos) {
    if (deltaMinutos <= 0) return 0;
    int total = hora * 60 + minuto + deltaMinutos;
    int diasPassados = total / 1440;
    int resto = total % 1440;
    hora = resto / 60;
    minuto = resto % 60;
    return diasPassados;
}

bool Calendario::isAnoBissexto(int a) {
    return (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
}

int Calendario::diasNoMes(int m, int a) {
    static const int tabela[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (m < 1 || m > 12) return 30;
    if (m == 2 && isAnoBissexto(a)) return 29;
    return tabela[m];
}

Calendario::Calendario(int d, int m, int a) : dia(d), mes(m), ano(a) {
    if (ano < 1970) ano = 2026;
    if (mes < 1 || mes > 12) mes = 1;
    int maxDias = diasNoMes(mes, ano);
    if (dia < 1 || dia > maxDias) dia = 1;
}

void Calendario::avancarDias(int dias) {
    for (int i = 0; i < dias; ++i) {
        dia++;
        if (dia > diasNoMes(mes, ano)) {
            dia = 1;
            mes++;
            if (mes > 12) {
                mes = 1;
                ano++;
            }
        }
    }
}

RelogioCalendario::RelogioCalendario(int d, int m, int a, int h, int min)
    : Relogio(h, min), Calendario(d, m, a), minutosAcumulados(h * 60 + min) {}

void RelogioCalendario::sincronizarMinutoDia(int minutoAbsoluto) {
    if (minutoAbsoluto > minutosAcumulados) {
        *this += (minutoAbsoluto - minutosAcumulados);
    }
}

RelogioCalendario& RelogioCalendario::operator++() {
    return (*this += 1);
}

RelogioCalendario& RelogioCalendario::operator+=(int deltaMinutos) {
    if (deltaMinutos <= 0) return *this;
    int dias = avancarMinutosRelogio(deltaMinutos);
    if (dias > 0) {
        avancarDias(dias);
    }
    minutosAcumulados += deltaMinutos;
    return *this;
}

bool RelogioCalendario::operator==(const RelogioCalendario& outro) const {
    return ano == outro.ano && mes == outro.mes && dia == outro.dia &&
           hora == outro.hora && minuto == outro.minuto;
}

std::string RelogioCalendario::formatarDataHora() const {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << dia << "/"
        << std::setw(2) << mes << "/"
        << std::setw(4) << ano << " "
        << std::setw(2) << hora << ":"
        << std::setw(2) << minuto;
    return oss.str();
}

std::string RelogioCalendario::formatarHora() const {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << hora << ":"
        << std::setw(2) << minuto;
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const RelogioCalendario& rc) {
    os << rc.formatarDataHora();
    return os;
}
