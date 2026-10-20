#ifndef MOTO_H
#define MOTO_H

#include "Veiculo.h"

class Moto : public Veiculo {
private:
    int cilindradas;

public:
    Moto(const std::string& p, const std::string& m, int entrada, int cc = 150);
    virtual ~Moto() override = default;

    virtual double calcularTarifa(int minutosPermanencia) const override;
    virtual std::string getTipo() const override { return "Moto"; }
    virtual int getVagasNecessarias() const override { return 1; }

    int getCilindradas() const { return cilindradas; }
};

#endif // MOTO_H
