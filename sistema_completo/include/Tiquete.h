#ifndef TIQUETE_H
#define TIQUETE_H

#include <string>
#include <iostream>
#include <iomanip>

/**
 * @brief Representa o tíquete de estacionamento emitido na entrada.
 */
class Tiquete {
private:
    int id;
    std::string placa;
    std::string tipoVeiculo;
    int minutoEntrada;
    int numeroVaga;
    std::string dataHoraEmissao;
    bool pago;

public:
    Tiquete(int i = 0,
            const std::string& p = "",
            const std::string& tipo = "",
            int entrada = 0,
            int vaga = -1,
            const std::string& dataHora = "")
        : id(i),
          placa(p),
          tipoVeiculo(tipo),
          minutoEntrada(entrada),
          numeroVaga(vaga),
          dataHoraEmissao(dataHora),
          pago(false) {}

    int getId() const { return id; }
    const std::string& getPlaca() const { return placa; }
    const std::string& getTipoVeiculo() const { return tipoVeiculo; }
    int getMinutoEntrada() const { return minutoEntrada; }
    int getNumeroVaga() const { return numeroVaga; }
    const std::string& getDataHoraEmissao() const { return dataHoraEmissao; }
    bool isPago() const { return pago; }

    void marcarComoPago() { pago = true; }

    bool operator==(const Tiquete& outro) const {
        return id == outro.id && placa == outro.placa;
    }

    friend std::ostream& operator<<(std::ostream& os, const Tiquete& t) {
        int minutoDia = t.minutoEntrada % 1440;
        if (minutoDia < 0) minutoDia = 0;
        int h = minutoDia / 60;
        int m = minutoDia % 60;

        char preenchimentoAnterior = os.fill();
        os << "[TIQUETE #" << t.id << " | Placa: " << t.placa
           << " | " << t.tipoVeiculo << " | Entrada: ";
        if (!t.dataHoraEmissao.empty()) {
            os << t.dataHoraEmissao;
        } else {
            os << std::setfill('0') << std::setw(2) << h << ":"
               << std::setfill('0') << std::setw(2) << m;
        }
        os << " | Vaga: " << t.numeroVaga << "]";
        os.fill(preenchimentoAnterior);
        return os;
    }
};

#endif // TIQUETE_H
