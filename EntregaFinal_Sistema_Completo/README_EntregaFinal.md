# Entrega Final – Sistema Integrado Smart Parking Controller
**Módulo:** Sistema Completo de Estacionamento Inteligente  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados (UFSC)  

---

## 1. Descrição Geral do Sistema
O **Smart Parking Controller** integra em um único núcleo **C++11** os principais conceitos da disciplina:
1. **`FSM_Controlador`**: Máquina de Estados Finitos determinística (6 estados, 7 eventos);
2. **`FilaEncadeada<T>` & `UtilitariosTemplate.h`**: Classe Template dinâmica própria (`new`/`delete`, RAII, *Deep Copy*) e Funções Template utilitárias;
3. **`Veiculo`, `Carro`, `Moto`, `Caminhao`**: Classe abstrata pura, polimorfismo dinâmico (*vtable*), destrutor virtual e sobrecarga de `operator<<` com `friend`;
4. **`RelogioCalendario`**: Herança múltipla (`Relogio` + `Calendario`) e sobrecarga de operadores (`++`, `+=`, `==`, `<<`);
5. **`IHardwareCancela` / `HardwareSimulado`**: Camada de Abstração de Hardware (HAL) desacoplando lógica e periféricos (servo, LEDs, buzzer, LCD 16x2);
6. **`WatchdogCancela`**: Monitoramento concorrente de *timeout* com `std::thread`, `std::mutex`, `std::lock_guard` e `std::atomic`;
7. **`Estacionamento`**: Orquestrador central integrando STL (`std::vector`, `std::map`, `<algorithm>`), persistência em arquivos (`<fstream>` CSV/TXT) e hierarquia completa de exceções (`Excecoes.h`).

## 2. Compilação e Execução
No diretório `EntregaFinal_Sistema_Completo`:
```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -O2 -pthread -static \
    src/FSM_Controlador.cpp src/Veiculo.cpp src/Carro.cpp src/Moto.cpp \
    src/Caminhao.cpp src/RelogioCalendario.cpp src/Estacionamento.cpp src/main.cpp \
    -o bin/smart_parking.exe
./bin/smart_parking.exe
```

Para executar em modo servidor IPC (usado pelo `simulador_visual.py`):
```bash
./bin/smart_parking.exe --pipe
```
