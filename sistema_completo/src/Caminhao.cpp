#include "../include/Caminhao.h"
#include "../include/UtilitariosTemplate.h"

Caminhao::Caminhao(const std::string& p, const std::string& m, int entrada, int eixos)
    : Veiculo(p, m, entrada), numeroEixos(limitarIntervalo(eixos, 2, 9)) {}

double Caminhao::calcularTarifa(int minutosPermanencia) const {
    if (minutosPermanencia <= 15) {
        return 0.0;
    }

    double tarifa = 15.0; // Tarifa base para caminhão pesado
    if (minutosPermanencia > 60) {
        int minutosExcedentes = minutosPermanencia - 60;
        int horasAdicionais = (minutosExcedentes + 59) / 60;
        tarifa += horasAdicionais * 5.0;
    }

    if (numeroEixos > 2) {
        tarifa += (numeroEixos - 2) * 2.50; // Adicional por eixo extra
    }

    return tarifa;
}
