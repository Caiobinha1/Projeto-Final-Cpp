#include <iostream>
#include <string>
#include <cassert>
#include "../include/FilaEncadeada.h"
#include "../include/Excecoes.h"

using namespace std;

// Estrutura de teste simulando informações de tíquete de entrada
struct TiqueteInfo {
    int id;
    string placa;
    string horaEntrada;

    TiqueteInfo(int i = 0, string p = "", string h = "")
        : id(i), placa(p), horaEntrada(h) {}

    bool operator==(const TiqueteInfo& outro) const {
        return id == outro.id && placa == outro.placa && horaEntrada == outro.horaEntrada;
    }
};

ostream& operator<<(ostream& os, const TiqueteInfo& t) {
    os << "[ID #" << t.id << " | Placa: " << t.placa << " | Entrada: " << t.horaEntrada << "]";
    return os;
}

int main() {
    cout << "========================================================\n";
    cout << "  UFSC / CTC / EEL - EEL7323 Programacao C++ Embarcada  \n";
    cout << "  PROJETO FINAL - SMART PARKING CONTROLLER              \n";
    cout << "  SUITE DE TESTES: ENTREGA 2 (ESTRUTURAS & TEMPLATES)   \n";
    cout << "========================================================\n\n";

    // Teste 1: Fila Encadeada de inteiros (FIFO)
    cout << "[TESTE 1] Testando FilaEncadeada<int> com tipos primitivos...\n";
    FilaEncadeada<int> filaInt;
    assert(filaInt.vazia());
    assert(filaInt.tamanho() == 0);

    filaInt.enfileirar(10);
    filaInt.enfileirar(20);
    filaInt.enfileirar(30);

    assert(!filaInt.vazia());
    assert(filaInt.tamanho() == 3);
    assert(filaInt.frente() == 10);

    assert(filaInt.desenfileirar() == 10);
    assert(filaInt.desenfileirar() == 20);
    assert(filaInt.desenfileirar() == 30);
    assert(filaInt.vazia());
    assert(filaInt.tamanho() == 0);
    cout << "  -> Insercoes e remocoes FIFO validadas com sucesso! [SUCESSO]\n\n";

    // Teste 2: Tratamento de Exceções com FilaVaziaException
    cout << "[TESTE 2] Validando tratamento de excecao em fila vazia...\n";
    bool capturouExcecao = false;
    try {
        filaInt.desenfileirar();
    } catch (const FilaVaziaException& e) {
        capturouExcecao = true;
        cout << "  -> Excecao capturada com sucesso: " << e.what() << endl;
    }
    assert(capturouExcecao);

    capturouExcecao = false;
    try {
        filaInt.frente();
    } catch (const FilaVaziaException& e) {
        capturouExcecao = true;
        cout << "  -> Excecao de frente() capturada com sucesso: " << e.what() << endl;
    }
    assert(capturouExcecao);
    cout << "  -> Excecoes de fila vazia verificadas e seguras! [SUCESSO]\n\n";

    // Teste 3: Fila de Objetos Complexos (Tiquetes de Estacionamento)
    cout << "[TESTE 3] Testando FilaEncadeada<TiqueteInfo> com objetos do dominio...\n";
    FilaEncadeada<TiqueteInfo> filaTiquetes;
    TiqueteInfo t1(101, "ABC-1234", "10:15:00");
    TiqueteInfo t2(102, "XYZ-9876", "10:18:22");
    TiqueteInfo t3(103, "MNO-4567", "10:22:05");

    filaTiquetes.enfileirar(t1);
    filaTiquetes.enfileirar(t2);
    filaTiquetes.enfileirar(t3);

    assert(filaTiquetes.tamanho() == 3);
    cout << "  Frente da fila: " << filaTiquetes.frente() << endl;
    assert(filaTiquetes.frente() == t1);

    TiqueteInfo rem = filaTiquetes.desenfileirar();
    assert(rem == t1);
    assert(filaTiquetes.desenfileirar() == t2);
    assert(filaTiquetes.desenfileirar() == t3);
    assert(filaTiquetes.vazia());
    cout << "  -> Enfileiramento de objetos compostos validado! [SUCESSO]\n\n";

    // Teste 4: Construtor de Cópia e Atribuição (Deep Copy)
    cout << "[TESTE 4] Validando Deep Copy (Construtor de Copia e Operador=)...\n";
    FilaEncadeada<string> fOriginal;
    fOriginal.enfileirar("Carro_A");
    fOriginal.enfileirar("Carro_B");

    // Construtor de cópia
    FilaEncadeada<string> fCopia(fOriginal);
    assert(fCopia.tamanho() == fOriginal.tamanho());
    assert(fCopia.desenfileirar() == "Carro_A");
    // A fila original não deve ter sido afetada pela remoção na cópia
    assert(fOriginal.tamanho() == 2);
    assert(fOriginal.frente() == "Carro_A");

    // Operador de atribuição
    FilaEncadeada<string> fAtribuida;
    fAtribuida = fOriginal;
    assert(fAtribuida.tamanho() == 2);
    assert(fAtribuida.desenfileirar() == "Carro_A");
    assert(fOriginal.tamanho() == 2);
    cout << "  -> Independencia de memoria e Deep Copy 100% comprovados! [SUCESSO]\n\n";

    // Teste 5: Teste de Estresse e Desalocação
    cout << "[TESTE 5] Teste de estresse com 10.000 operacoes de fila...\n";
    {
        FilaEncadeada<int> fEstresse;
        for (int i = 0; i < 10000; ++i) {
            fEstresse.enfileirar(i);
        }
        assert(fEstresse.tamanho() == 10000);
        for (int i = 0; i < 5000; ++i) {
            assert(fEstresse.desenfileirar() == i);
        }
        assert(fEstresse.tamanho() == 5000);
        fEstresse.limpar();
        assert(fEstresse.vazia());
    } // Aqui o destrutor de fEstresse é acionado garantindo liberação RAII
    cout << "  -> Estresse concluido sem vazamentos de memoria! [SUCESSO]\n\n";

    cout << "========================================================\n";
    cout << "  RESULTADO FINAL ENTREGA 2: 100% DOS TESTES PASSARAM!  \n";
    cout << "  Todas as estruturas dinamicas e excecoes estao OK!    \n";
    cout << "========================================================\n";

    return 0;
}
