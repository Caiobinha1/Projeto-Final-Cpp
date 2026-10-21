# Projeto Final – Smart Parking Controller (Sistema de Estacionamento Inteligente)
**Universidade Federal de Santa Catarina (UFSC) – Centro Tecnológico (CTC)**  
**Departamento de Engenharia Elétrica e Eletrônica (EEL)**  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson Wedderhoff Spengler  
**Semestre:** 2026/2  

---

## 1. Visão Geral do Projeto

O **Smart Parking Controller** é um projeto de engenharia de software para sistemas embarcados em tempo real, desenvolvido em C++11 com arquitetura orientada a objetos. O sistema gerencia o fluxo de controle de acesso, estacionamento e cobrança de veículos através de:

- **Máquina de Estados Finitos (FSM):** Controle determinístico da cancela de entrada com sensores de laço indutivo, botão do totem, barreira óptica e temporizador de segurança;
- **Estruturas de Dados Dinâmicas Próprias:** Fila encadeada baseada em **Templates** com alocação dinâmica manual (`new`/`delete`), cópia profunda (*Deep Copy*) e padrão RAII (sem dependência exclusiva da STL);
- **Hierarquia Polimórfica de Veículos:** Classe base abstrata pura `Veiculo` e subclasses derivadas `Carro`, `Moto` e `Caminhao` com *late binding* via *vtable*, calculando tarifas específicas de forma extensível e desacoplada;
- **Sobrecarga de Operadores e `friend`:** Serialização formatada em streams (`operator<<`) com acesso seguro a membros protegidos;
- **Tratamento Robusto de Exceções:** Hierarquia customizada baseada em `std::exception` para falhas operacionais, lotação máxima e tíquetes inválidos;
- **Simulador Gráfico Interativo (Dashboard / Jogo):** Interface visual em Python/Tkinter conectada diretamente ao binário C++ via Pipe IPC em tempo real.

---

## 2. Todas as Funcionalidades do Sistema

Para oferecer uma visão clara e completa de toda a engenharia implementada no projeto, abaixo estão detalhadas as **9 principais funcionalidades operacionais e técnicas** do Smart Parking Controller:

1. **Controle Físico de Acesso por FSM (Cancela Inteligente):**
   - Gerencia a cancela de entrada através de uma Máquina de Estados Finitos (`FSM_Controlador`) determinística (`FECHADA`, `ABRINDO`, `ABERTA`, `FECHANDO`, `ALERTA_OBSTACULO`);
   - Monitora eventos assíncronos: presença de veículo no laço indutivo, acionamento do botão do totem, barreira óptica antiesmagamento e temporizador de fechamento automático com tratamento de *timeout* e falhas mecânicas.

2. **Fila Dinâmica de Espera (Buffer FIFO com Templates C++):**
   - Estrutura de dados genérica `FilaEncadeada<T>` implementada do zero com alocação dinâmica em *heap* (`new`/`delete`);
   - Gerencia a fila de espera na cancela quando o pátio atinge a capacidade máxima de vagas;
   - Implementa construtor de cópia profunda (*Deep Copy*), operador de atribuição (`operator=`) e destrutor para assegurar o ciclo de vida sob o paradigma RAII (*Resource Acquisition Is Initialization*), impedindo vazamentos de memória.

3. **Alocação Inteligente de Vagas Físicas no Pátio:**
   - O pátio possui 10 vagas físicas numeradas (índices 0 a 9) gerenciadas por um vetor polimórfico de ponteiros `Veiculo* vagas[10]`;
   - **Vagas Simples:** Carros e motos ocupam 1 vaga individual;
   - **Vagas Contíguas Duplas:** Caminhões exigem e alocam automaticamente **duas vagas livres consecutivas lado a lado** (ex: vagas 3 e 4). Na saída, ambas as vagas são liberadas simultaneamente.

4. **Cálculo de Tarifas 100% Polimórfico (Sem `switch/case`):**
   - Elimina verificações condicionais procedurais, utilizando classes derivadas com *late binding* via tabela de ponteiros de funções virtuais (*vtable*):
     - **Tolerância Geral:** Permanências de até 15 minutos são isentas de cobrança para qualquer categoria de veículo;
     - **Carro:** Tarifa base de R$ 10,00/hora. Veículos com tag eletrônica (Sem Parar/ConectCar) recebem **10% de desconto automático**;
     - **Moto:** Tarifa base de R$ 5,00/hora. Motos de alta cilindrada (> 300cc) possuem **adicional de 20%** por porte e taxa de seguro de vaga;
     - **Caminhão:** Tarifa base de R$ 20,00/hora multiplicada pelo número de eixos e proporcional às vagas ocupadas.

5. **Emissão de Tíquetes e Registro de Saída:**
   - Ao passar pela cancela, o sistema emite um tíquete com identificador sequencial único, placa do veículo, categoria e horário de entrada em minutos;
   - No momento do pagamento e saída, o sistema valida o tíquete, calcula o tempo total de permanência e computa a tarifa devida de acordo com as regras polimórficas do veículo.

6. **Tratamento de Exceções Operacionais com Hierarquia Própria:**
   - Hierarquia customizada de tratamento de erros baseada em `std::exception`:
     - `EstacionamentoLotadoException`: disparada ao tentar estacionar com pátio cheio e fila externa esgotada;
     - `FilaVaziaException`: disparada ao tentar desenfileirar sem veículos aguardando;
     - `TiqueteInvalidoException`: disparada ao tentar registrar saída com tíquete nulo ou inexistente.
   - Previne travamentos e garante a integridade dos dados mesmo sob condições operacionais adversas.

7. **Relatório Gerencial e Fechamento Financeiro em Tempo Real:**
   - Monitoramento contínuo da operação com métricas instantâneas:
     - Total de vagas livres e ocupadas;
     - Quantidade de veículos na fila de espera externa;
     - Contagem histórica de veículos atendidos por categoria (carros, motos e caminhões);
     - Receita financeira total acumulada (R$) e faturamento discriminado.
   - Impressão formatada em console através da sobrecarga do operador `<<` (`std::ostream& operator<<`).

8. **Simulador Gráfico Interativo com IPC em Tempo Real:**
   - Interface visual desktop em Python/Tkinter ([`simulador_visual.py`](./simulador_visual.py)) com renderização gráfica do pátio e das 10 vagas coloridas por status e categoria do veículo;
   - Comunicação bidirecional via Pipes (`stdin`/`stdout`) com o binário compilado em C++ (`smart_parking.exe`), garantindo que **toda a lógica de negócios, regras e cálculos sejam executados estritamente pelo código C++**;
   - Permite acelerar o tempo do relógio da simulação, injetar veículos e testar todas as transições de estados da cancela interativamente.

9. **Firmware e Projeto Físico de Hardware Embarcado:**
   - Projeto prático para implementação física com microcontrolador Arduino Uno ou ESP32;
   - Integração com servo motor SG90 (cancela física), sensor ultrassônico HC-SR04 (laço indutivo), sensor infravermelho de obstáculo (fotocélula antiesmagamento), LEDs de sinalização e display LCD I2C para mensagens ao motorista;
   - Código firmware pronto ([`firmware_arduino_cancela.ino`](./Implementacao_Fisica_Hardware/firmware_arduino_cancela.ino)) e esquemático elétrico pino a pino disponível na pasta de hardware.

---

## 3. Como Funcionam as Entregas do Projeto (Metodologia Prof. Eduardo Bezerra)

Conforme as diretrizes pedagógicas da disciplina e as orientações encaminhadas pelo docente, a segunda metade do semestre é estruturada no formato de **laboratório prático de desenvolvimento assistido**, no qual o projeto é avaliado de forma contínua em **4 etapas cumulativas**:

### A. A Transição de Fase na Disciplina
Encerradas as provas teóricas e práticas individuais da primeira etapa, a disciplina adota o modelo de mentoria de engenharia:
- **Trabalho em Dupla:** Desenvolvimento conjunto da arquitetura, código e testes (máximo 2 alunos);
- **Aulas de Acompanhamento:** Menos encontros expositivos tradicionais e mais sessões de laboratório para validação dos marcos e entregas parciais;
- **Foco no Ciclo Completo:** Desde o levantamento de requisitos e diagramação preliminar até a codificação, testes de integração, documentação formal e apresentação prática ao vivo.

### B. As 4 Etapas de Entrega e Critérios de Avaliação
1. **Marco 0 – Apresentação da Proposta (Semana 10):**
   - Apresentação da proposta técnica, mapeamento dos conceitos de OO aplicados, especificação de requisitos (funcionais e não-funcionais), divisão de tarefas (EAP/WBS) e cronograma de execução.
2. **Marco 1 – Modelagem e Núcleo da FSM (Semana 11-12 | Peso 20%):**
   - Diagrama de classes preliminar e implementação da Máquina de Estados Finitos (`FSM_Controlador`) funcional em C++, com testes comprovando o controle da cancela e transições seguras.
3. **Marco 2 – Estruturas de Dados Encadeadas (Semana 13 | Peso 20%):**
   - Implementação de estruturas dinâmicas próprias (filas/listas encadeadas com Templates) e hierarquia de tratamento de exceções, operando com alocação dinâmica (`new`/`delete`) e RAII.
4. **Marco 3 – Polimorfismo e Modelo Tarifário (Semana 14 | Peso 20%):**
   - Hierarquia polimórfica com classes abstratas, eliminando comandos condicionais (`switch/case`), com despacho dinâmico por *vtable*, sobrecarga de operadores e testes unitários.
5. **Marco 4 – Sistema Integral Consolidado & Apresentação (Semanas 15-17 | Peso 40%):**
   - Integração completa de todos os módulos na classe `Estacionamento`, repositório GitHub documentado, bateria completa de testes automatizados, relatório técnico final em PDF e demonstração prática ao vivo para os professores.

### C. Checklist Obrigatório de Conceitos de C++ Exigidos
O Prof. Bezerra avalia se a solução contempla explicitamente os pilares da disciplina:
* [x] **Classes Abstratas e Polimorfismo Dinâmico:** Classes com funções virtuais puras (`= 0`) e despacho em tempo de execução via *vtable*;
* [x] **Destrutor Virtual e RAII:** Destrutores virtuais em todas as classes base polimórficas para evitar vazamentos de memória na destruição de subclasses;
* [x] **Máquina de Estados Finitos (FSM):** Controle determinístico de atuadores e tratamento de falhas/timeouts;
* [x] **Templates (Generics):** Estruturas parametrizadas por tipo com segurança estática em tempo de compilação;
* [x] **Estruturas Dinâmicas Próprias:** Fila encadeada implementada manualmente com gerenciamento na *heap*, sem dependência de bibliotecas prontas;
* [x] **Sobrecarga de Operadores e `friend`:** Funções amigas permitindo acesso controlado a membros protegidos para inserção em streams;
* [x] **Tratamento de Exceções:** Classes de erro herdando de `std::exception` capturadas por blocos `try/catch`.

### D. Por que o Smart Parking Controller é o Projeto Ideal?
Na programação procedural clássica (C estruturado), sistemas desse porte dependem de tabelas gigantes de `switch/case` e structs com uniões inseguras (`void*`). Em C++, a hierarquia de `Veiculo` permite que o sistema gerencie o pátio com o vetor `Veiculo* vagas[10]`: novas categorias de veículos podem ser adicionadas no futuro sem que uma única linha do controlador central precise ser alterada, respeitando com perfeição o **Princípio Aberto/Fechado (Open/Closed Principle - OCP)**.

---

## 4. Estrutura de Diretórios do Projeto

```
.
├── README.md                                  <- Este documento mestre
├── simulador_visual.py                        <- Simulador gráfico interativo (GUI / Jogo)
│
├── Proposta/                                  <- Proposta Inicial (Apresentação Semana 10)
│   ├── Proposta_Projeto_Estacionamento.pdf    <- Relatório em PDF da proposta
│   ├── README_Proposta.md
│   └── gerar_proposta_pdf.py
│
├── Entrega1_FSM_Modelagem/                    <- Marco 1: Máquina de Estados da Cancela (20%)
│   ├── include/FSM_Controlador.h
│   ├── src/FSM_Controlador.cpp
│   ├── src/main_entrega1.cpp                  <- Testes da FSM
│   ├── bin/entrega1.exe                       <- Binário compilado e validado
│   ├── Relatorio_Entrega1.pdf                 <- Relatório técnico em PDF
│   └── README_Entrega1.md
│
├── Entrega2_Estruturas_Dados/                 <- Marco 2: Fila Encadeada Template & Exceções (20%)
│   ├── include/FilaEncadeada.h                <- Template com alocação dinâmica
│   ├── include/Excecoes.h                     <- Hierarquia de exceções
│   ├── src/main_entrega2.cpp                  <- Testes de estresse e Deep Copy
│   ├── bin/entrega2.exe                       <- Binário compilado e validado
│   ├── Relatorio_Entrega2.pdf                 <- Relatório técnico em PDF
│   └── README_Entrega2.md
│
├── Entrega3_Polimorfismo_Tarifas/             <- Marco 3: Polimorfismo Dinâmico & Tarifas (20%)
│   ├── include/Veiculo.h, Carro.h, Moto.h, Caminhao.h
│   ├── src/Veiculo.cpp, Carro.cpp, Moto.cpp, Caminhao.cpp
│   ├── src/main_entrega3.cpp                  <- Testes de vtable e cálculo tarifário
│   ├── bin/entrega3.exe                       <- Binário compilado e validado
│   ├── Relatorio_Entrega3.pdf                 <- Relatório técnico em PDF
│   └── README_Entrega3.md
│
├── EntregaFinal_Sistema_Completo/             <- Marco 4: Sistema Integral Consolidado (40%)
│   ├── include/Estacionamento.h               <- Classe central orquestradora
│   ├── include/ (todos os headers integrados)
│   ├── src/Estacionamento.cpp
│   ├── src/main.cpp                           <- Bateria completa de testes automatizados e modo pipe
│   ├── bin/smart_parking.exe                  <- Binário final consolidado
│   ├── Relatorio_Final_Projeto.pdf            <- Relatório técnico final consolidado
│   └── README_EntregaFinal.md
│
└── Implementacao_Fisica_Hardware/             <- Anexo Prático: Circuito Físico & Embarcados
    ├── firmware_arduino_cancela.ino           <- Firmware C++ para Arduino/ESP32
    ├── README.md                              <- Tutorial completo passo a passo (Pinout e Peças BR)
    ├── Tutorial_Montagem_Hardware.pdf         <- Relatório técnico em PDF com esquema elétrico
    └── gerar_tutorial_pdf.py
```

---

## 5. Relatórios Técnicos em PDF

Cada entrega possui seu respectivo relatório técnico formal gerado em PDF, pronto para entrega no Moodle ou impressão:

1. **[Proposta_Projeto_Estacionamento.pdf](./Proposta/Proposta_Projeto_Estacionamento.pdf)**: Proposta de desenvolvimento com justificativa de OO, requisitos, EAP e cronograma.
2. **[Relatorio_Entrega1.pdf](./Entrega1_FSM_Modelagem/Relatorio_Entrega1.pdf)**: Relatório da Entrega 1 (Máquina de estados da cancela, tabela de transições e validação).
3. **[Relatorio_Entrega2.pdf](./Entrega2_Estruturas_Dados/Relatorio_Entrega2.pdf)**: Relatório da Entrega 2 (Fila encadeada template, RAII, deep copy e tratamento de exceções).
4. **[Relatorio_Entrega3.pdf](./Entrega3_Polimorfismo_Tarifas/Relatorio_Entrega3.pdf)**: Relatório da Entrega 3 (Hierarquia polimórfica de veículos, despacho por *vtable* e modelo tarifário).
5. **[Relatorio_Final_Projeto.pdf](./EntregaFinal_Sistema_Completo/Relatorio_Final_Projeto.pdf)**: Documentação técnica final consolidada com a suíte completa de testes de integração.
6. **[Tutorial_Montagem_Hardware.pdf](./Implementacao_Fisica_Hardware/Tutorial_Montagem_Hardware.pdf)**: Guia completo de prototipagem física com lista de compras no Brasil e esquema pino a pino.

---

## 6. Simulador Visual Interativo (Testador Gráfico / Jogo)

Para facilitar a validação, demonstração ao professor e testes visuais com sua dupla, foi criado o script [`simulador_visual.py`](./simulador_visual.py).

### Como Funciona:
- Trata-se de uma interface gráfica desktop (desenvolvida com `tkinter`, já nativo do Python);
- O simulador **conecta-se diretamente com o executável C++ compilado (`smart_parking.exe`)** através de um canal IPC via Pipe (`stdin`/`stdout`);
- **Todo o processamento é executado pelo motor C++**: a máquina de estados (FSM), a alocação de vagas, a fila encadeada e o cálculo tarifário polimórfico;
- **Recursos Visuais:**
  - Pátio gráfico com 10 vagas desenhadas (verde quando livres; azul, laranja ou roxo com placas e modelos quando ocupadas);
  - Animação da cancela física (aberta/fechada) e semáforo com sinal luminoso;
  - Botões para simular a chegada de Carros, Motos e Caminhões;
  - Emissão de tíquetes e liberação física de passagem;
  - Pagamento e desocupação de vagas com emissão de recibo detalhado;
  - Relógio de simulação acelerável (+15 min, +30 min, +1 hora);
  - Terminal inferior exibindo em tempo real os comandos trocados com o binário C++.

### Como Executar o Simulador:
```bash
python simulador_visual.py
```

---

## 7. Instruções de Compilação e Execução dos Códigos C++

Todos os programas foram desenvolvidos para compilação com `g++ (GCC)` sob padrão `C++11`:

### A. Executar a Suíte Completa de Testes de Integração (Recomendado):
```bash
cd EntregaFinal_Sistema_Completo
g++ -std=c++11 -Wall src/FSM_Controlador.cpp src/Veiculo.cpp src/Carro.cpp src/Moto.cpp src/Caminhao.cpp src/Estacionamento.cpp src/main.cpp -o bin/smart_parking.exe
./bin/smart_parking.exe
```

### B. Compilar e Testar os Módulos Isoladamente:

* **Módulo 1 (FSM):**
  ```bash
  cd Entrega1_FSM_Modelagem
  g++ -std=c++11 -Wall src/FSM_Controlador.cpp src/main_entrega1.cpp -o bin/entrega1.exe
  ./bin/entrega1.exe
  ```

* **Módulo 2 (Estruturas Dinâmicas e Templates):**
  ```bash
  cd Entrega2_Estruturas_Dados
  g++ -std=c++11 -Wall src/main_entrega2.cpp -o bin/entrega2.exe
  ./bin/entrega2.exe
  ```

* **Módulo 3 (Polimorfismo e Tarifas):**
  ```bash
  cd Entrega3_Polimorfismo_Tarifas
  g++ -std=c++11 -Wall src/Veiculo.cpp src/Carro.cpp src/Moto.cpp src/Caminhao.cpp src/main_entrega3.cpp -o bin/entrega3.exe
  ./bin/entrega3.exe
  ```

---

## 8. Status e Validação dos Testes

| Módulo / Entrega | Compilador | Advertências / Erros | Testes Automatizados | Gerenciamento de Memória |
| :--- | :---: | :---: | :---: | :---: |
| **Entrega 1 (FSM)** | `g++ -std=c++11` | 0 erros, 0 warnings | **100% Aprovado** | Seguro |
| **Entrega 2 (Templates)** | `g++ -std=c++11` | 0 erros, 0 warnings | **100% Aprovado** | Seguro (Deep Copy / RAII) |
| **Entrega 3 (Polimorfismo)** | `g++ -std=c++11` | 0 erros, 0 warnings | **100% Aprovado** | Seguro (Destrutor Virtual) |
| **Entrega Final (Integrada)** | `g++ -std=c++11` | 0 erros, 0 warnings | **100% Aprovado** | Zero Memory Leaks |
