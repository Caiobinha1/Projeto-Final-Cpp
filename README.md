# Smart Parking Controller (C++11)

**UFSC – CTC – EEL7323 / EEL510265:** Programação C++ para Sistemas Embarcados (2026/2)  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson Wedderhoff Spengler  
**Autores:** Caio Missfeld Carlos e Fábio Augusto Dittrich  

> **Relatório Técnico Final (PDF):** [`Relatorio_Final_Smart_Parking.pdf`](./Relatorio_Final_Smart_Parking.pdf)  
> **Manual do Simulador 2D:** [`MANUAL_SIMULADOR.md`](./MANUAL_SIMULADOR.md)

---

## Visão Geral

Controlador de estacionamento de 10 vagas em **C++11** orientado a objetos, organizado em **módulos isolados testáveis** e integrado no **sistema completo**, acompanhado de um simulador interativo 2D em pixel art ([`simulador_visual.py`](./simulador_visual.py)) conectado ao executável C++ via *Pipe IPC* (`stdin`/`stdout`).

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
├── Jogar_Simulador.bat                 # Duplo clique no Windows para abrir o simulador 2D
├── Relatorio_Final_Smart_Parking.pdf   # Relatório técnico final (PDF)
├── relatorio_latex/                    # Fontes LaTeX (main.tex, references.bib)
├── MANUAL_SIMULADOR.md                 # Guia rápido do simulador 2D
├── Makefile                            # Compilação e testes de todos os blocos
├── simulador_visual.py                 # Simulador 2D pixel-art (Python/Tkinter + IPC)
├── modulos/
│   ├── 01_fsm_cancela/                 # Bloco 1: FSM da cancela (include, src, bin)
│   ├── 02_fila_template_excecoes/      # Bloco 2: FilaEncadeada<T> e exceções
│   └── 03_polimorfismo_tarifas/        # Bloco 3: Hierarquia Veiculo e tarifas
├── sistema_completo/                   # Projeto final integrado (include, src, bin)
└── tests/test_ipc_e2e.py               # Teste end-to-end do protocolo Pipe IPC
```

---

## Como Rodar e Testar

### 1. Abrir o Simulador 2D (Jogo Interativo)
- **Windows:** dê **duplo clique** em **`Jogar_Simulador.bat`** (ou rode `python simulador_visual.py`).
- **Linux / macOS:** compile com `make sistema` e rode `python3 simulador_visual.py`.

### 2. Compilar e Testar Todos os Blocos + Sistema Completo
```bash
make test          # Linux / macOS
mingw32-make test  # Windows (MinGW-w64)
```

### 3. Testar Cada Bloco Separadamente
- **Bloco 1 – FSM da Cancela:** `make modulo1 && ./modulos/01_fsm_cancela/bin/teste_fsm`
- **Bloco 2 – Fila Template e Exceções:** `make modulo2 && ./modulos/02_fila_template_excecoes/bin/teste_fila`
- **Bloco 3 – Polimorfismo e Tarifas:** `make modulo3 && ./modulos/03_polimorfismo_tarifas/bin/teste_polimorfismo`
- **Sistema Completo (10 suítes integradas):** `make sistema && ./sistema_completo/bin/smart_parking`
- **Teste End-to-End do Pipe IPC:** `python tests/test_ipc_e2e.py`
