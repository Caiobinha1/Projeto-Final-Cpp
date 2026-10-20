#ifndef CAMINHAO_H
#define CAMINHAO_H

#include "Veiculo.h"

class Caminhao : public Veiculo {
private:
    int numeroEixos;

public:
    Caminhao(const std::string& p, const std::string& m, int entrada, int eixos = 2);
    virtual ~Caminhao() override = default;

    virtual double calcularTarifa(int minutosPermanencia) const override;
    virtual std::string getTipo() const override { return "Caminhao"; }
    virtual int getVagasNecessarias() const override { return 2; } // Ocupa duas vagas

    int getNumeroEixos() const { return numeroEixos; }
};

#endif // CAMINHAO_H
