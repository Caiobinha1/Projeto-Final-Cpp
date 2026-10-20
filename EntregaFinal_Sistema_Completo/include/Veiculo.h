#ifndef VEICULO_H
#define VEICULO_H

#include <string>
#include <iostream>

/**
 * @brief Classe base abstrata representando um veículo no estacionamento.
 * 
 * Demonstra os conceitos de:
 * 1. Classe Abstrata (métodos virtuais puros);
 * 2. Destrutor Virtual para desalocação polimórfica correta;
 * 3. Membros protegidos (protected) herdados pelas subclasses;
 * 4. Sobrecarga de operadores com função friend.
 */
class Veiculo {
protected:
    std::string placa;
    std::string modelo;
    int minutoEntrada; // Minutos desde 00:00 (ex: 10:30 = 630 min)

public:
    Veiculo(const std::string& p, const std::string& m, int entrada);
    virtual ~Veiculo() = default; // Destrutor virtual essencial

    // Métodos virtuais puros (Polimorfismo dinâmico)
    virtual double calcularTarifa(int minutosPermanencia) const = 0;
    virtual std::string getTipo() const = 0;
    virtual int getVagasNecessarias() const = 0;

    // Métodos concretos de consulta
    std::string getPlaca() const { return placa; }
    std::string getModelo() const { return modelo; }
    int getMinutoEntrada() const { return minutoEntrada; }

    // Sobrecarga de operador com friend para exibição formatada
    friend std::ostream& operator<<(std::ostream& os, const Veiculo& v);
};

#endif // VEICULO_H
