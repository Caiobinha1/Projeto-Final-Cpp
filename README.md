# Smart Parking Controller (C++11)

**UFSC – CTC – EEL7323 / EEL510265:** Programação C++ para Sistemas Embarcados (2026/2)  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson Wedderhoff Spengler  
**Autores:** Caio Missfeld Carlos e Fábio Augusto Dittrich  

> **Relatório Técnico Final (PDF):** [`Relatorio_Final_Smart_Parking.pdf`](./Relatorio_Final_Smart_Parking.pdf)  
> **Manual do Simulador 2D:** [`MANUAL_SIMULADOR.md`](./MANUAL_SIMULADOR.md)

---

## Visão Geral

Controlador de estacionamento de 10 vagas em **C++11** orientado a objetos, acompanhado de um simulador interativo 2D em pixel art ([`simulador_visual.py`](./simulador_visual.py)) conectado diretamente ao executável C++ via *Pipe IPC* (`stdin`/`stdout`).

### Conteúdos da Disciplina Aplicados
| Conceito | Implementação |
| :--- | :--- |
| **Encapsulamento, `const`/`static` e FSM** | `FSM_Controlador` (6 estados, 7 eventos, transições determinísticas) |
| **Composição, Agregação e RAII** | `Estacionamento` gerenciando `Veiculo* vagas[10]`, fila, FSM e desalocação |
| **Classes Abstratas e Polimorfismo** | `Veiculo` (`= 0`, destrutor virtual) $\rightarrow$ `Carro`, `Moto`, `Caminhao` (*vtable*) |
| **Herança Múltipla e Operadores** | `RelogioCalendario` (`Relogio` + `Calendario`), `++`, `+=`, `==` e `friend <<` |
| **Templates e Alocação Dinâmica** | `FilaEncadeada<T>` (`new`/`delete`, *Deep Copy*) e `UtilitariosTemplate.h` |
| **Exceções Customizadas** | `Excecoes.h` herdando de `std::exception` (5 exceções específicas) |
| **STL e Arquivos (`<fstream>`)** | `std::vector`, `std::map`, `std::max_element`, escrita/leitura CSV e TXT |
| **Camada HAL (Embarcados)** | `IHardwareCancela` + `HardwareSimulado` (servo, semáforo, buzzer, LCD 16x2) |
| **Concorrência (`std::thread`)** | `WatchdogCancela` com `std::mutex`, `std::lock_guard` e `std::atomic` |

---

## Estrutura do Repositório

```
.
├── Relatorio_Final_Smart_Parking.pdf   # Relatório técnico final (PDF)
├── Relatorio_Final_LaTeX/              # Fontes LaTeX (main.tex, references.bib)
├── MANUAL_SIMULADOR.md                 # Guia rápido do simulador 2D
├── Makefile                            # Compilação e testes de todos os blocos
├── simulador_visual.py                 # Simulador 2D pixel-art (Python/Tkinter + IPC)
├── Entrega1_FSM_Modelagem/             # Bloco 1: FSM isolada (include, src, bin)
├── Entrega2_Estruturas_Dados/          # Bloco 2: FilaEncadeada<T> e exceções
├── Entrega3_Polimorfismo_Tarifas/      # Bloco 3: Hierarquia Veiculo e tarifas
├── EntregaFinal_Sistema_Completo/      # Projeto completo integrado
└── tests/test_ipc_e2e.py               # Teste end-to-end do protocolo Pipe IPC
```

---

## Como Compilar e Testar

### 1. Rodar o Simulador 2D Interativo (recomendado)
```bash
python simulador_visual.py
```
*(No Windows, `EntregaFinal_Sistema_Completo/bin/smart_parking.exe` já está compilado com `-static`. Em Linux/macOS, compile primeiro com `make final`.)*

### 2. Compilar e Testar Tudo via `Makefile`
```bash
make test          # Linux / macOS
mingw32-make test  # Windows (MinGW-w64)
```

### 3. Compilar e Rodar Cada Bloco Separadamente (`g++` C++11)
- **Sistema Completo (10 suítes integradas):**
  ```bash
  make final && ./EntregaFinal_Sistema_Completo/bin/smart_parking
  ```
- **Bloco 1 – FSM:**
  ```bash
  make entrega1 && ./Entrega1_FSM_Modelagem/bin/entrega1
  ```
- **Bloco 2 – Fila Template e Exceções:**
  ```bash
  make entrega2 && ./Entrega2_Estruturas_Dados/bin/entrega2
  ```
- **Bloco 3 – Polimorfismo e Tarifas:**
  ```bash
  make entrega3 && ./Entrega3_Polimorfismo_Tarifas/bin/entrega3
  ```
- **Teste End-to-End do Pipe IPC:**
  ```bash
  python tests/test_ipc_e2e.py
  ```
