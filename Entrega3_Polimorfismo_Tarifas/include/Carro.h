#ifndef CARRO_H
#define CARRO_H

#include "Veiculo.h"

class Carro : public Veiculo {
private:
    bool temTagEletronica; // Se possui tag de pedágio/estacionamento (10% desconto)

public:
    Carro(const std::string& p, const std::string& m, int entrada, bool temTag = false);
    virtual ~Carro() override = default;

    virtual double calcularTarifa(int minutosPermanencia) const override;
    virtual std::string getTipo() const override { return "Carro"; }
    virtual int getVagasNecessarias() const override { return 1; }

    bool getTemTag() const { return temTagEletronica; }
};

#endif // CARRO_H
