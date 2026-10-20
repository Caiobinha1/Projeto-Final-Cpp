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
    bool pago;

public:
    Tiquete(int i = 0, const std::string& p = "", const std::string& tipo = "", int entrada = 0, int vaga = -1)
        : id(i), placa(p), tipoVeiculo(tipo), minutoEntrada(entrada), numeroVaga(vaga), pago(false) {}

    int getId() const { return id; }
    std::string getPlaca() const { return placa; }
    std::string getTipoVeiculo() const { return tipoVeiculo; }
    int getMinutoEntrada() const { return minutoEntrada; }
    int getNumeroVaga() const { return numeroVaga; }
    bool isPago() const { return pago; }

    void marcarComoPago() { pago = true; }

    friend std::ostream& operator<<(std::ostream& os, const Tiquete& t) {
        int h = t.minutoEntrada / 60;
        int m = t.minutoEntrada % 60;
        os << "[TIQUETE #" << t.id << " | Placa: " << t.placa
           << " | " << t.tipoVeiculo << " | Entrada: "
           << std::setfill('0') << std::setw(2) << h << ":"
           << std::setfill('0') << std::setw(2) << m
           << " | Vaga: " << t.numeroVaga << "]";
        return os;
    }
};

#endif // TIQUETE_H
