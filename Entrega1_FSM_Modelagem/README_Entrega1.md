# Entrega 1 – Modelagem e Máquina de Estados Finitos (FSM)
**Módulo:** Controlador da Cancela Automática de Entrada  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  

---

## 1. Descrição do Módulo
Implementação da classe `FSM_Controlador`, responsável por gerenciar os estados operacionais da cancela e responder aos eventos gerados pelos sensores de presença, botões e sensores de travessia.

## 2. Estrutura de Arquivos
- `include/FSM_Controlador.h`: Declaração dos enums de estado e eventos, e interface da classe `FSM_Controlador`.
- `src/FSM_Controlador.cpp`: Implementação da lógica de transições de estado com validação determinística.
- `src/main_entrega1.cpp`: Suíte de testes automatizados com validação por asserções (`assert`).
- `bin/entrega1.exe`: Executável compilado e validado.
- `Relatorio_Entrega1.pdf`: Relatório técnico formatado com tabela de estados e logs de execução.

## 3. Instruções de Compilação e Execução
No diretório `Entrega1_FSM_Modelagem`:
```bash
g++ -std=c++11 -Wall src/FSM_Controlador.cpp src/main_entrega1.cpp -o bin/entrega1.exe
./bin/entrega1.exe
```

## 4. Resultados Obtidos
- **100% dos testes unitários aprovados**:
  - Transições válidas em ciclo normal;
  - Bloqueio estrito de eventos ilegais;
  - Tratamento de timeout com transição para modo de alerta;
  - Desistência do veículo tratada com retorno ao estado livre.
