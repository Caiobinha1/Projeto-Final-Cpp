# Projeto Final – Smart Parking Controller (Sistema de Estacionamento Inteligente)
**Universidade Federal de Santa Catarina (UFSC) – Centro Tecnológico (CTC)**  
**Departamento de Engenharia Elétrica e Eletrônica (EEL)**  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson Wedderhoff Spengler  
**Autores:** Caio Missfeld Carlos e Fábio Augusto Dittrich  
**Semestre:** 2026/2  

---

## 1. Visão Geral do Projeto

O **Smart Parking Controller** é um projeto completo de engenharia de software em **C++11** desenvolvido para aplicar, de forma integrada e simulável, o máximo do conteúdo programático da disciplina EEL7323/EEL510265. O sistema gerencia o fluxo completo de controle de acesso, fila externa de espera, alocação física de vagas, tarifação polimórfica, controle temporal, auditoria em arquivo e supervisão de segurança em tempo real:

- **Máquina de Estados Finitos (FSM):** Controle determinístico da cancela (`FSM_Controlador`) com 6 estados e 7 eventos assíncronos;
- **Templates (Classes e Funções):** Fila encadeada dinâmica própria (`FilaEncadeada<T>` / `Nodo<T>`) com alocação manual (`new`/`delete`), cópia profunda (*Deep Copy*) e RAII, complementada por funções *template* utilitárias (`UtilitariosTemplate.h`);
- **Hierarquia Polimórfica de Veículos:** Classe base abstrata pura `Veiculo` e subclasses `Carro`, `Moto` e `Caminhao` com *late binding* via *vtable* e destrutor virtual;
- **Herança Múltipla e Sobrecarga de Operadores:** Classe `RelogioCalendario` derivando de `Relogio` e `Calendario`, com operadores `++`, `+=`, `==` e `friend operator<<` (também presente em `Veiculo` e `Tiquete`);
- **Camada de Abstração de Hardware (HAL):** Interface abstrata `IHardwareCancela` e driver simulado `HardwareSimulado` (servo motor 0°–90°, semáforo LED, buzzer e display LCD 16x2), desacoplando a lógica de negócio dos periféricos físicos;
- **STL e Persistência em Arquivos (`<fstream>`):** Histórico de movimentações (`std::vector`), consolidação por categoria (`std::map`), busca de maior permanência (`std::max_element`) e exportação/leitura de relatórios em `.csv` e `.txt`;
- **Concorrência (`std::thread` + `std::mutex`):** Monitor *watchdog* (`WatchdogCancela`) executando em *thread* dedicada com `std::lock_guard` e `std::atomic<bool>` para disparo automático de `ALERTA_BLOQUEIO` por *timeout*;
- **Tratamento Robusto de Exceções:** Hierarquia própria herdada de `std::exception` (`FilaVaziaException`, `EstacionamentoLotadoException`, `TiqueteInvalidoException`, `CancelaBloqueadaException` e `PlacaDuplicadaException`);
- **Simulador Gráfico Interativo (IPC):** Interface visual em Python/Tkinter (`simulador_visual.py`) conectada em tempo real ao executável C++ via *Pipes* (`stdin`/`stdout`), com renderização do pátio, cancela e display LCD 16x2 virtual da HAL.

---

## 2. Funcionalidades do Sistema e Mapeamento da Disciplina

1. **Controle Determinístico da Cancela (`FSM_Controlador`):**
   - Estados oficiais implementados: `ESTADO_LIVRE`, `ESTADO_VEICULO_DETECTADO`, `ESTADO_EMITINDO_TIQUETE`, `ESTADO_CANCELA_ABERTA`, `ESTADO_PASSAGEM_CONCLUIDA` e `ESTADO_ALERTA_BLOQUEIO`;
   - Eventos tratados: `EVENTO_VEICULO_CHEGOU`, `EVENTO_BOTAO_PRESSIONADO`, `EVENTO_TIQUETE_RETIRADO`, `EVENTO_SENSOR_PASSAGEM`, `EVENTO_TIMEOUT_PASSAGEM`, `EVENTO_RESET_SISTEMA` e `EVENTO_VEICULO_RECUOU`;
   - Sincronismo automático com a fila de espera: ao concluir a travessia de um veículo, o próximo veículo da fila avança imediatamente para o laço indutivo (`ESTADO_VEICULO_DETECTADO`).

2. **Fila Dinâmica de Espera e Utilitários Genéricos (`FilaEncadeada<T>` e `UtilitariosTemplate.h`):**
   - Fila FIFO com `Nodo<T>` alocado manualmente em *heap* (`new`/`delete`), destrutor RAII, construtor de cópia e `operator=` com *Deep Copy*;
   - Funções *template* `limitarIntervalo<T>`, `calcularMedia<T>` e `encontrarMaximo<T>`.

3. **Alocação Física de Vagas e Tarifação 100% Polimórfica (`Veiculo`, `Carro`, `Moto`, `Caminhao`):**
   - Vetor polimórfico `Veiculo* vagas[10]` (Carros e Motos ocupam 1 vaga; Caminhões exigem e liberam **2 vagas contíguas**);
   - Regras tarifárias implementadas via *vtable* (`calcularTarifa(int)`):
     - **Tolerância Geral:** permanência $\le 15$ min é isenta (`R$ 0,00`) para todas as categorias;
     - **Carro:** `R$ 10,00` (1ª hora) + `R$ 3,00` por hora excedente (fração arredondada para cima); **10% de desconto** com tag eletrônica;
     - **Moto:** `R$ 5,00` (1ª hora) + `R$ 1,50` por hora excedente; adicional de **`R$ 2,00`** para motores acima de `500 cc`;
     - **Caminhão:** `R$ 15,00` (1ª hora) + `R$ 5,00` por hora excedente; adicional de **`R$ 2,50`** por eixo excedente acima de 2 eixos.

4. **Relógio e Calendário Integrados (`RelogioCalendario` — Herança Múltipla):**
   - Herda simultaneamente de `Relogio` e `Calendario`, propagando viradas de meia-noite e anos bissextos com sobrecarga de `operator++`, `operator+=`, `operator==` e `friend operator<<`.

5. **Camada de Abstração de Hardware (`IHardwareCancela` / `HardwareSimulado`):**
   - Interface abstrata pura controlando servo motor ($0^\circ$ / $90^\circ$), semáforo LED, buzzer e display LCD 16x2, refletidos em tempo real no simulador visual e prontos para portabilidade embarcada.

6. **Análise Gerencial (STL) e Persistência em Arquivos (`<fstream>`):**
   - Histórico em `std::vector<RegistroMovimentacao>`, estatísticas por categoria em `std::map`, maior permanência com `std::max_element` e exportação/leitura validada de arquivos `.csv` e `.txt`.

7. **Watchdog Concorrente (`WatchdogCancela` — `std::thread` + `std::mutex`):**
   - Supervisão assíncrona em *thread* separada com exclusão mútua (`std::lock_guard<std::mutex>`) e parada limpa (`std::atomic<bool>` + `join()`), acionando `ALERTA_BLOQUEIO` automaticamente em caso de estouro de tempo com a cancela aberta.

8. **Hierarquia Customizada de Exceções (`Excecoes.h`):**
   - `EstacionamentoException : public std::exception` e 5 exceções específicas: `FilaVaziaException`, `EstacionamentoLotadoException`, `TiqueteInvalidoException`, `CancelaBloqueadaException` e `PlacaDuplicadaException`.

9. **Simulador Gráfico Interativo (`simulador_visual.py`) + Anexo de Hardware (`Implementacao_Fisica_Hardware/`):**
   - Interface Tkinter conectada via Pipe IPC (`smart_parking.exe --pipe`), com controles de chegada, emissão, travessia, *timeout*, *reset*, recuo, avanço de relógio, exportação de relatório CSV/TXT e display LCD 16x2 virtual.

---

## 3. Estrutura de Diretórios do Projeto

```
.
├── README.md                                  <- Documentação principal do repositório
├── MANUAL_SIMULADOR.md                        <- Guia detalhado de uso do simulador 2D pixel-art
├── Makefile                                   <- Build e testes automatizados (Windows/Linux)
├── simulador_visual.py                        <- Simulador interativo 2D em pixel art (Python/Tkinter + IPC)
│
├── Relatorio_Final_LaTeX/                     <- Relatório Técnico Final em LaTeX (Template Prof. Bezerra)
│   ├── main.tex                               <- Documento em 2 colunas com diagramas TikZ e código real
│   ├── references.bib                         <- Referências bibliográficas da disciplina
│   └── Relatorio_Final_Smart_Parking.pdf      <- PDF final compilado pronto para entrega
│
├── tests/                                     <- Testes end-to-end do protocolo Pipe IPC
│   └── test_ipc_e2e.py
│
├── Proposta/                                  <- Proposta Inicial
├── Entrega1_FSM_Modelagem/                    <- Marco 1: Máquina de Estados da Cancela (FSM)
├── Entrega2_Estruturas_Dados/                 <- Marco 2: Fila Encadeada Template & Exceções
├── Entrega3_Polimorfismo_Tarifas/             <- Marco 3: Polimorfismo Dinâmico & Tarifas
│
├── EntregaFinal_Sistema_Completo/             <- Marco 4: Sistema Integral Consolidado
│   ├── include/
│   │   ├── Estacionamento.h                   <- Orquestrador central (STL, Arquivos, Mutex, HAL)
│   │   ├── FSM_Controlador.h                  <- Máquina de Estados Finitos
│   │   ├── FilaEncadeada.h                    <- Classe Template + RAII + Deep Copy
│   │   ├── UtilitariosTemplate.h              <- Funções Template utilitárias
│   │   ├── Veiculo.h, Carro.h, Moto.h, Caminhao.h
│   │   ├── Tiquete.h                          <- Tíquete com carimbo temporal e operator<<
│   │   ├── RelogioCalendario.h                <- Herança Múltipla (Relogio + Calendario)
│   │   ├── HardwareHAL.h                      <- Interface abstrata HAL + HardwareSimulado
│   │   ├── WatchdogCancela.h                  <- Thread de supervisão de timeout (std::thread)
│   │   └── Excecoes.h                         <- Hierarquia de exceções (std::exception)
│   ├── src/                                   <- Implementações (.cpp) e suíte de 10 testes + modo --pipe
│   └── bin/smart_parking.exe                  <- Binário final compilado estaticamente (-static)
│
└── Implementacao_Fisica_Hardware/             <- Anexo Opcional: Firmware Arduino/ESP32 e tutorial
```

---

## 4. Como Compilar, Testar e Executar

### A. Compilar e Rodar Todos os Testes via `Makefile`
```bash
make test
```

### B. Compilar Manualmente o Sistema Completo (`g++` C++11)
No diretório `EntregaFinal_Sistema_Completo`:
```bash
g++ -std=c++11 -Wall -Wextra -Wpedantic -O2 -pthread -static \
    src/FSM_Controlador.cpp src/Veiculo.cpp src/Carro.cpp src/Moto.cpp \
    src/Caminhao.cpp src/RelogioCalendario.cpp src/Estacionamento.cpp src/main.cpp \
    -o bin/smart_parking.exe
./bin/smart_parking.exe
```
*(Em Linux/macOS, omita a flag `-static` e a extensão `.exe`.)*

### C. Executar o Simulador Visual Interativo
```bash
python simulador_visual.py
```

### D. Compilar o Relatório Final em LaTeX
A pasta [`Relatorio_Final_LaTeX/`](./Relatorio_Final_LaTeX/) contém [`main.tex`](./Relatorio_Final_LaTeX/main.tex) e [`references.bib`](./Relatorio_Final_LaTeX/references.bib), prontos para importação direta no **Overleaf** ou compilação local com `pdflatex` + `bibtex`.

---

## 5. Status de Compilação e Testes

| Módulo / Entrega | Compilação (`g++ -std=c++11 -Wall -Wextra -Wpedantic`) | Testes Automatizados |
| :--- | :---: | :---: |
| **Entrega 1 (FSM)** | 0 erros, 0 warnings | **5/5 Aprovados (100%)** |
| **Entrega 2 (Templates e Exceções)** | 0 erros, 0 warnings | **5/5 Aprovados (100%)** |
| **Entrega 3 (Polimorfismo e Tarifas)** | 0 erros, 0 warnings | **5/5 Aprovados (100%)** |
| **Entrega Final (Sistema Integrado)** | 0 erros, 0 warnings | **10/10 Suítes Aprovadas (100%)** |
| **Integração Pipe IPC (`--pipe`)** | Protocolo JSON + HAL validado | **100% Aprovado** |
