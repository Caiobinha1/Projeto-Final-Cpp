# Entrega 3 – Polimorfismo Dinâmico e Modelo Tarifário
**Módulo:** Hierarquia de Classes de Veículos e Cálculo Tarifário  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  

---

## 1. Descrição do Módulo
Implementação da hierarquia polimórfica de veículos encabeçada pela classe base abstrata pura `Veiculo` e suas subclasses derivadas `Carro`, `Moto` e `Caminhao`. Utilização de *late binding* via *vtable*, destrutores virtuais com padrão RAII, membros `protected` e sobrecarga de operadores com funções `friend`.

## 2. Estrutura de Arquivos
- `include/Veiculo.h`: Declaração da classe base abstrata pura `Veiculo`.
- `include/Carro.h`, `include/Moto.h`, `include/Caminhao.h`: Declaração das subclasses.
- `src/Veiculo.cpp`, `src/Carro.cpp`, `src/Moto.cpp`, `src/Caminhao.cpp`: Implementação dos métodos e regras de tarifação.
- `src/main_entrega3.cpp`: Suíte de testes unitários com vetor heterogêneo de ponteiros da classe base.
- `bin/entrega3.exe`: Executável compilado e validado.
- `Relatorio_Entrega3.pdf`: Relatório técnico formatado com tabela tarifária e logs de execução.

## 3. Instruções de Compilação e Execução
No diretório `Entrega3_Polimorfismo_Tarifas`:
```bash
g++ -std=c++11 -Wall src/Veiculo.cpp src/Carro.cpp src/Moto.cpp src/Caminhao.cpp src/main_entrega3.cpp -o bin/entrega3.exe
./bin/entrega3.exe
```

## 4. Resultados Obtidos
- **100% dos testes aprovados**:
  - Eliminação de `switch/case` por despacho dinâmico polimórfico;
  - Tolerância de 15 minutos calculada com sucesso;
  - Tarifação de 1 hora e horas adicionais fracionadas validada;
  - Destruição polimórfica sem vazamentos de memória (RAII);
  - Formatação uniforme com `operator<<`.
