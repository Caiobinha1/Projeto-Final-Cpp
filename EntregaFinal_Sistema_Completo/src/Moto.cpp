#include "../include/Moto.h"
#include "../include/UtilitariosTemplate.h"

Moto::Moto(const std::string& p, const std::string& m, int entrada, int cc)
    : Veiculo(p, m, entrada), cilindradas(limitarIntervalo(cc, 50, 2500)) {}

double Moto::calcularTarifa(int minutosPermanencia) const {
    if (minutosPermanencia <= 15) {
        return 0.0;
    }

    double tarifa = 5.0; // Tarifa base primeira hora
    if (minutosPermanencia > 60) {
        int minutosExcedentes = minutosPermanencia - 60;
        int horasAdicionais = (minutosExcedentes + 59) / 60;
        tarifa += horasAdicionais * 1.50;
    }

    if (cilindradas > 500) {
        tarifa += 2.0; // Adicional porte grande
    }

    return tarifa;
}
