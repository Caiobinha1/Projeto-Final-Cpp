# Proposta de Projeto Final – Smart Parking Controller
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  
**Departamento:** Engenharia Elétrica e Eletrônica (EEL / CTC / UFSC)  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler  

---

## 1. Visão Geral da Proposta
Desenvolvimento de um **Controlador de Estacionamento Inteligente (*Smart Parking Controller*)** em C++ orientado a objetos, voltado para sistemas embarcados. O sistema controla a cancela automatizada por meio de uma Máquina de Estados Finitos (FSM), aloca vagas polimórficas de forma dinâmica, calcula tarifas diferenciadas por categoria de veículo e enfileira eventos e tíquetes em uma fila encadeada com template.

## 2. Conceitos de Orientação a Objetos em C++ Aplicados
1. **Classes Abstratas e Polimorfismo Dinâmico:** Classe base `Veiculo` com método virtual puro `calcularTarifa(tempo) = 0` e subclasses `Carro`, `Moto` e `Caminhao`.
2. **Destrutor Virtual e RAII:** Liberação de memória segura e polimórfica com `virtual ~Veiculo() = default;`.
3. **Máquina de Estados Finitos (FSM):** Classe `FSM_Controlador` para controle das transições da cancela.
4. **Templates (Generics):** Estrutura `FilaEncadeada<T>` parametrizada por tipo com gerenciamento dinâmico próprio (`new`/`delete`).
5. **Sobrecarga de Operadores e `friend`:** `operator<<` para exibição formatada de tíquetes e cupons.
6. **Tratamento de Exceções:** Classes derivadas de `std::exception` para falhas operacionais e de capacidade.

## 3. Arquivo PDF Gerado
- [Proposta_Projeto_Estacionamento.pdf](file:///C:/Programacao/C/Projeto%20final/Proposta/Proposta_Projeto_Estacionamento.pdf)
