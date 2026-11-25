#include "../include/Veiculo.h"
#include <iomanip>

Veiculo::Veiculo(const std::string& p, const std::string& m, int entrada)
    : placa(p), modelo(m), minutoEntrada(entrada) {}

std::ostream& operator<<(std::ostream& os, const Veiculo& v) {
    int minutoDia = v.minutoEntrada % 1440;
    if (minutoDia < 0) minutoDia = 0;
    int horas = minutoDia / 60;
    int minutos = minutoDia % 60;

    char preenchimentoAnterior = os.fill();
    os << "[" << v.getTipo() << "] Placa: " << v.placa
       << " | Modelo: " << v.modelo
       << " | Entrada: " << std::setfill('0') << std::setw(2) << horas << ":"
       << std::setfill('0') << std::setw(2) << minutos;
    os.fill(preenchimentoAnterior);
    return os;
}
