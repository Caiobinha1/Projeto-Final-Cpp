#include "../include/Carro.h"
#include <cmath>

Carro::Carro(const std::string& p, const std::string& m, int entrada, bool temTag)
    : Veiculo(p, m, entrada), temTagEletronica(temTag) {}

double Carro::calcularTarifa(int minutosPermanencia) const {
    if (minutosPermanencia <= 15) {
        return 0.0; // Tolerância
    }

    double tarifa = 10.0; // Primeira hora
    if (minutosPermanencia > 60) {
        int minutosExcedentes = minutosPermanencia - 60;
        int horasAdicionais = (minutosExcedentes + 59) / 60; // Arredonda para cima
        tarifa += horasAdicionais * 3.0;
    }

    if (temTagEletronica) {
        tarifa *= 0.90; // 10% de desconto
    }

    return tarifa;
}
