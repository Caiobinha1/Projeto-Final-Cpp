# Manual de Uso – Simulador Interativo Pixel-Art (`simulador_visual.py`)

**Projeto:** Smart Parking Controller (UFSC – EEL7323 / EEL510265)  
**Autores:** Caio Missfeld Carlos e Fábio Augusto Dittrich  
**Repositório:** https://github.com/Caiobinha1/Projeto-Final-Cpp

---

## 1. Como o Simulador Funciona

O simulador [`simulador_visual.py`](./simulador_visual.py) é uma interface gráfica 2D interativa em **pixel art** (desenvolvida em Python 3 com `tkinter` nativo, sem dependências externas) que funciona como um jogo de gerenciamento de estacionamento (*Smart Parking Tycoon*).

Toda a lógica do sistema — máquina de estados da cancela (FSM), fila encadeada genérica (`FilaEncadeada<T>`), alocação de vagas, cálculo tarifário polimórfico (*vtable*), relógio/calendário (`RelogioCalendario`), camada de hardware (HAL), estatísticas STL, exceções e exportação de arquivos — **é executada em tempo real pelo binário C++11 compilado** (`sistema_completo/bin/smart_parking.exe`), conectado por *Pipe IPC* (`stdin`/`stdout`).

---

## 2. Como Abrir o Simulador

- **No Windows (mais rápido):** dê **duplo clique em [`Jogar_Simulador.bat`](./Jogar_Simulador.bat)** na pasta raiz do projeto.
- **Via terminal (Windows / Linux / macOS):**
  ```bash
  python simulador_visual.py
  ```
- **Pré-requisitos:** Python 3 instalado (com `tkinter`, já incluso no instalador padrão). O binário `sistema_completo/bin/smart_parking.exe` já vem compilado estaticamente (`-static`). Caso altere o código C++, recompile com:
  ```bash
  mingw32-make sistema   # Windows
  make sistema           # Linux / macOS
  ```

---

## 3. Conhecendo a Tela do Simulador

- **Barra Superior (HUD):**
  - Exibe a **Data e Hora atuais** sincronizadas com a classe `RelogioCalendario` (herança múltipla C++);
  - Botão **`▶ Modo Jogo Auto: OFF/ON`**: ativa a chegada periódica automática de veículos aleatórios na fila externa para operar o estacionamento como um jogo em tempo real.
- **Painel Esquerdo (Controles do Simulador):** dividido em 6 blocos que permitem acionar todas as funcionalidades do motor C++.
- **Área Central (Mapa 2D Pixel-Art):**
  - **Display LCD 16x2 Virtual (HAL C++):** mostra no canto superior esquerdo exatamente as 2 linhas enviadas pela camada `HardwareSimulado`;
  - **Guarita, Semáforo, Sirene e Cancela Animada:** a cancela listrada rotaciona suavemente de `0°` (fechada) a `90°` (aberta) conforme o ângulo do servo da HAL; em `ALERTA_BLOQUEIO`, a sirene pisca indicando acionamento do buzzer;
  - **Fila Externa (`FilaEncadeada<Veiculo*>`):** veículos aguardando antes da cancela aparecem enfileirados no canto inferior esquerdo;
  - **Pátio com 10 Vagas:** vagas verdes estão livres; vagas ocupadas exibem o sprite em pixel art da categoria (**Carro** azul/vermelho/verde/prata com marcação dourada quando possui Tag, **Moto** laranja 250cc ou vermelho/ouro >500cc, e **Caminhão** roxo largo ocupando visualmente **2 vagas contíguas**).
- **Rodapé (Terminal IPC ao Vivo):** mostra cada comando enviado ao executável C++ e a resposta retornada em tempo real.

---

## 4. Controles, Cliques e Atalhos de Teclado

### A. Botões do Painel de Controle
| Seção | Botão / Controle | Ação no Motor C++ |
| :--- | :--- | :--- |
| **1. Chegada** | `Carro` / `Carro c/ Tag (-10%)` | Instancia `new Carro(...)` com `temTagEletronica = false/true` e enfileira na `FilaEncadeada<Veiculo*>`. |
| **1. Chegada** | `Moto 250cc` / `Moto 650cc (+R$2)` | Instancia `new Moto(...)` testando o adicional de alta cilindrada (`> 500 cc`). |
| **1. Chegada** | `Caminhao 2e` / `Caminhao 4e (2 vag)` | Instancia `new Caminhao(...)` exigindo **2 vagas livres contíguas** e adicional por eixo extra. |
| **2. Cancela (FSM)** | `Emitir Tíquete & Abrir Cancela` | Dispara `BOTAO_PRESSIONADO` $\rightarrow$ `TIQUETE_RETIRADO`, aloca vaga(s) e abre o servo (`90°`). |
| **2. Cancela (FSM)** | `Cruzar Cancela & Fechar` | Dispara `SENSOR_PASSAGEM`, fecha a cancela (`0°`) e avança o próximo da fila automaticamente. |
| **2. Cancela (FSM)** | `Timeout` | Com a cancela aberta, dispara `EVENTO_TIMEOUT_PASSAGEM` $\rightarrow$ entra em `ALERTA_BLOQUEIO` e liga o buzzer. |
| **2. Cancela (FSM)** | `Reset` | Dispara `EVENTO_RESET_SISTEMA`, desarma o alarme e restaura a operação. |
| **2. Cancela (FSM)** | `Recuar` | Simula desistência do primeiro veículo da fila (`EVENTO_VEICULO_RECUOU`), removendo-o da memória (RAII). |
| **3. Saída** | Lista + `Registrar Saída e Cobrar` | Executa `v->calcularTarifa(tempo)` via *vtable*, libera 1 ou 2 vagas, chama destrutor virtual e atualiza STL/faturamento. |
| **4. Relógio** | `+15m`, `+30m`, `+1h`, `+12h` | Avança `RelogioCalendario` (`operator+=`), inclusive virando o dia no calendário após `23:59`. |
| **5. Exceções & Arquivos** | `Testar Duplicada` / `Placa Inexistente` | Dispara e exibe ao vivo `PlacaDuplicadaException` e `TiqueteInvalidoException`. |
| **5. Exceções & Arquivos** | `Exportar Relatório CSV & TXT` | Grava `relatorio_smart_parking.csv` e `relatorio_smart_parking.txt` via `std::ofstream`. |

### B. Interação Direta com o Mouse no Pátio
- **Clique simples** em qualquer veículo estacionado: seleciona esse veículo na lista de saída.
- **Duplo clique** em qualquer veículo estacionado: realiza o pagamento e libera a(s) vaga(s) imediatamente, exibindo o valor cobrado flutuando sobre a vaga (`+R$ 16.00` ou `ISENTO (<=15m)`).

### C. Atalhos de Teclado
- **`1`** — Chegar Carro aleatório (normal ou com Tag)
- **`2`** — Chegar Moto aleatória (250cc ou 650cc)
- **`3`** — Chegar Caminhão aleatório (2, 3 ou 4 eixos)
- **`Espaço`** — Alterna entre *Emitir Tíquete/Abrir* e *Cruzar Cancela/Fechar*
- **`Enter`** — Cruzar Cancela / Fechar
- **`T`** — Simular Timeout (`ALERTA_BLOQUEIO`)
- **`R`** — Resetar Alerta (`EVENTO_RESET_SISTEMA`)
- **`+`** — Avançar relógio em `+30 min`

---

## 5. Roteiro Sugerido para Apresentação (Testando Toda a Matéria)

Siga esta sequência rápida (~3 minutos) para demonstrar todos os conteúdos da disciplina ao vivo:

1. **Fila Template & Sincronismo da FSM:** clique em `Carro c/ Tag (-10%)`, `Moto 650cc` e `Caminhao 4e` seguidamente. Observe `Fila: 3` e `FSM: VEICULO_DETECTADO` no painel e no LCD da HAL.
2. **Proteção de Cancela Aberta (`CancelaBloqueadaException`):** clique em `Emitir Tíquete & Abrir` (observe a cancela abrir para `90°`, LED verde e o carro dirigindo até a Vaga 1). Clique novamente em `Emitir Tíquete` *antes* de fechar para exibir a exceção C++ bloqueando a operação.
3. **Passagem e Alocação de 2 Vagas Contíguas:** clique em `Cruzar Cancela` (a FSM avança automaticamente para o próximo da fila). Repita para a Moto (Vaga 2) e para o Caminhão (observe o sprite largo ocupando as **Vagas 3-4**).
4. **Timeout e Alerta de Segurança:** faça chegar mais um carro, clique em `Emitir Tíquete` e pressione **`T` (`Timeout`)**. A cancela entra em `ALERTA_BLOQUEIO`, o LCD exibe `ALERTA TIMEOUT!` e a sirene pisca. Pressione **`R` (`Reset`)** para restaurar.
5. **Polimorfismo Tarifário + Tolerância + Relógio/Calendário:**
   - Dê **duplo clique na Moto** com menos de 15 minutos decorridos $\rightarrow$ `ISENTO (<=15m)`.
   - Clique duas vezes em `+1h` (ou `+12h` duas vezes para mostrar a **virada de data** do `RelogioCalendario`).
   - Dê **duplo clique no Carro c/ Tag** (desconto de 10%) e no **Caminhão 4 eixos** (libera as 2 vagas simultaneamente e soma o adicional por eixos).
6. **Exceções e Arquivos (`<fstream>`):** clique em `Testar Duplicada`, `Placa Inexistente` e por fim em **`Exportar Relatório CSV & TXT`** para gerar os arquivos de auditoria no disco.
7. **Modo Jogo Livre:** clique em `▶ Modo Jogo Auto: ON` para demonstrar o fluxo contínuo em tempo real.
