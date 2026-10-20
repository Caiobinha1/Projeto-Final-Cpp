#include "../include/Veiculo.h"
#include <iomanip>

Veiculo::Veiculo(const std::string& p, const std::string& m, int entrada)
    : placa(p), modelo(m), minutoEntrada(entrada) {}

std::ostream& operator<<(std::ostream& os, const Veiculo& v) {
    int horas = v.minutoEntrada / 60;
    int minutos = v.minutoEntrada % 60;

    os << "[" << v.getTipo() << "] Placa: " << v.placa
       << " | Modelo: " << v.modelo
       << " | Entrada: " << std::setfill('0') << std::setw(2) << horas << ":"
       << std::setfill('0') << std::setw(2) << minutos;
    return os;
}
