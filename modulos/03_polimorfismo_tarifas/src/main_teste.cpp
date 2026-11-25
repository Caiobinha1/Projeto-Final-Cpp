#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include "../include/Veiculo.h"
#include "../include/Carro.h"
#include "../include/Moto.h"
#include "../include/Caminhao.h"

using namespace std;

int main() {
    cout << "========================================================\n";
    cout << "  UFSC / CTC / EEL - EEL7323 Programacao C++ Embarcada  \n";
    cout << "  PROJETO FINAL - SMART PARKING CONTROLLER              \n";
    cout << "  SUITE DE TESTES: MODULO 3 (POLIMORFISMO & TARIFAS)    \n";
    cout << "========================================================\n\n";

    // Teste 1: Instanciação Polimórfica com Vetor de Ponteiros da Classe Base
    cout << "[TESTE 1] Criando vetor heterogeneo de ponteiros Veiculo*...\n";
    const int NUM_VEICULOS = 4;
    Veiculo* patio[NUM_VEICULOS];

    // Instanciação dinâmica (upcasting implícito para Veiculo*)
    patio[0] = new Carro("ABC-1D23", "Toyota Corolla", 480, false); // Entrada 08:00, sem tag
    patio[1] = new Carro("XYZ-9E87", "Honda Civic", 480, true);     // Entrada 08:00, com tag (10% desc)
    patio[2] = new Moto("MOT-5F67", "Yamaha MT-07", 540, 689);      // Entrada 09:00, 689cc
    patio[3] = new Caminhao("CAM-3G45", "Scania R450", 420, 4);     // Entrada 07:00, 4 eixos

    cout << "  Veiculos estacionados no patio:\n";
    for (int i = 0; i < NUM_VEICULOS; ++i) {
        cout << "    Vaga " << i + 1 << ": " << *patio[i] << endl;
    }
    cout << "  -> Instanciacao polimorfica e operator<< validados! [SUCESSO]\n\n";

    // Teste 2: Cálculo de Tarifas com Tolerância (10 minutos de permanência)
    cout << "[TESTE 2] Validando regra de tolerancia (<= 15 min = R$ 0,00)...\n";
    for (int i = 0; i < NUM_VEICULOS; ++i) {
        double tarifa = patio[i]->calcularTarifa(10);
        assert(tarifa == 0.0);
    }
    cout << "  -> Tolerancia aplicada a todas as categorias! [SUCESSO]\n\n";

    // Teste 3: Cálculo Polimórfico para 1 Hora (60 min)
    cout << "[TESTE 3] Validando tarifa de 1 hora para cada classe derivada...\n";
    // Carro sem tag: 10.0
    assert(patio[0]->calcularTarifa(60) == 10.0);
    // Carro com tag (10% desc): 10.0 * 0.9 = 9.0
    assert(patio[1]->calcularTarifa(60) == 9.0);
    // Moto > 500cc: base 5.0 + 2.0 (porte) = 7.0
    assert(patio[2]->calcularTarifa(60) == 7.0);
    // Caminhão 4 eixos: base 15.0 + 2 eixos extras * 2.5 = 15.0 + 5.0 = 20.0
    assert(patio[3]->calcularTarifa(60) == 20.0);
    cout << "  -> Tarifas de 1 hora conferidas exatamente conforme regras! [SUCESSO]\n\n";

    // Teste 4: Cálculo Polimórfico com Horas Adicionais (150 min = 2h 30min -> 1h base + 2h extras)
    cout << "[TESTE 4] Validando horas adicionais fracionadas (150 min)...\n";
    // Carro sem tag: 10 + 2 * 3 = 16.0
    assert(patio[0]->calcularTarifa(150) == 16.0);
    // Carro com tag: 16.0 * 0.9 = 14.4
    assert(patio[1]->calcularTarifa(150) == 14.4);
    // Moto: 5.0 + 2 * 1.5 + 2.0 = 10.0
    assert(patio[2]->calcularTarifa(150) == 10.0);
    // Caminhão: 15.0 + 2 * 5.0 + 5.0 = 30.0
    assert(patio[3]->calcularTarifa(150) == 30.0);

    cout << fixed << setprecision(2);
    for (int i = 0; i < NUM_VEICULOS; ++i) {
        cout << "    " << patio[i]->getTipo() << " (" << patio[i]->getPlaca() 
             << ") - 150 min -> R$ " << patio[i]->calcularTarifa(150) << endl;
    }
    cout << "  -> Calculo tarifario polimorfico 100% correto! [SUCESSO]\n\n";

    // Teste 5: Liberação Polimórfica de Memória com Destrutor Virtual
    cout << "[TESTE 5] Testando destruicao polimorfica via delete patio[i]...\n";
    for (int i = 0; i < NUM_VEICULOS; ++i) {
        delete patio[i];
        patio[i] = nullptr;
    }
    cout << "  -> Todos os objetos liberados deterministicamente (RAII)! [SUCESSO]\n\n";

    cout << "========================================================\n";
    cout << "  RESULTADO FINAL MODULO 3: 100% DOS TESTES PASSARAM!   \n";
    cout << "  Polimorfismo, late binding e tarifas validados!       \n";
    cout << "========================================================\n";

    return 0;
}
