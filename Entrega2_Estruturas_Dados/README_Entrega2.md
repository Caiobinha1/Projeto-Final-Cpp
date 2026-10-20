# Entrega 2 – Estruturas de Dados Dinâmicas e Tratamento de Exceções
**Módulo:** Fila Encadeada Baseada em Template e Classes de Exceção  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  

---

## 1. Descrição do Módulo
Implementação manual da classe template `FilaEncadeada<T>` com gerenciamento explícito de memória na *heap* (`new`/`delete`), assegurando o padrão RAII e suporte à cópia profunda (*Deep Copy*). Integração com o subsistema de tratamento de erros baseado na hierarquia de `EstacionamentoException` (derivada de `std::exception`).

## 2. Estrutura de Arquivos
- `include/FilaEncadeada.h`: Declaração e definição da classe template `FilaEncadeada<T>` e struct `Nodo<T>`.
- `include/Excecoes.h`: Hierarquia de classes de exceção (`FilaVaziaException`, `EstacionamentoLotadoException`, `TiqueteInvalidoException`).
- `src/main_entrega2.cpp`: Suíte de testes unitários cobrindo tipos primitivos, objetos complexos, captura de exceções, Deep Copy e teste de estresse com 10.000 nós.
- `bin/entrega2.exe`: Executável compilado e validado.
- `Relatorio_Entrega2.pdf`: Relatório técnico formatado com análise de complexidade e logs de validação.

## 3. Instruções de Compilação e Execução
No diretório `Entrega2_Estruturas_Dados`:
```bash
g++ -std=c++11 -Wall src/main_entrega2.cpp -o bin/entrega2.exe
./bin/entrega2.exe
```

## 4. Resultados Obtidos
- **100% dos testes aprovados**:
  - Comportamento FIFO rigorosamente verificado;
  - Lançamento e captura determinística de `FilaVaziaException`;
  - Suporte a objetos de domínio (`TiqueteInfo`);
  - Deep Copy verificado (cópia independente sem compartilhamento indevido de nós);
  - Desalocação completa comprovada sem vazamentos de memória.
