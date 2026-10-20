# Entrega Final – Sistema Integrado Smart Parking Controller
**Módulo:** Sistema Completo de Estacionamento Inteligente  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  

---

## 1. Descrição Geral do Sistema
O **Smart Parking Controller** é um sistema completo e integrado de controle de acesso e tarifação de estacionamento, desenvolvido em C++11 com arquitetura orientada a objetos para ambientes embarcados.

### Módulos Integrados:
1. **`FSM_Controlador`**: Máquina de Estados Finitos que controla atuadores e sensores da cancela de acesso;
2. **`FilaEncadeada<T>`**: Estrutura dinâmica própria baseada em Template para fila de espera na cancela (sem uso da STL);
3. **`Veiculo` e Subclasses (`Carro`, `Moto`, `Caminhao`)**: Hierarquia polimórfica com classes abstratas puras, *late binding* via *vtable*, sobrecarga de `operator<<` com `friend` e destrutores virtuais com padrão RAII;
4. **`Estacionamento`**: Classe orquestradora central que gerencia o pátio de vagas heterogêneo (`Veiculo* vagas[10]`), emissão de tíquetes, faturamento financeiro e tratamento de exceções operacionais (`EstacionamentoLotadoException`, `FilaVaziaException`, `TiqueteInvalidoException`).

## 2. Estrutura de Arquivos
- `include/`:
  - `Estacionamento.h`: Interface da classe central;
  - `FSM_Controlador.h`: Estados e eventos da cancela;
  - `FilaEncadeada.h`: Template de fila encadeada dinâmica;
  - `Excecoes.h`: Hierarquia de classes de exceção derivadas de `std::exception`;
  - `Veiculo.h`, `Carro.h`, `Moto.h`, `Caminhao.h`: Classes de domínio;
  - `Tiquete.h`: Objeto representativo do tíquete de entrada.
- `src/`:
  - `Estacionamento.cpp`, `FSM_Controlador.cpp`, `Veiculo.cpp`, `Carro.cpp`, `Moto.cpp`, `Caminhao.cpp`;
  - `main.cpp`: Suíte completa de testes automatizados de integração.
- `bin/smart_parking.exe`: Executável consolidado testado com 100% de sucesso.
- `Relatorio_Final_Projeto.pdf`: Documento técnico final de engenharia.

## 3. Instruções de Compilação e Execução
No diretório `EntregaFinal_Sistema_Completo`:
```bash
g++ -std=c++11 -Wall src/FSM_Controlador.cpp src/Veiculo.cpp src/Carro.cpp src/Moto.cpp src/Caminhao.cpp src/Estacionamento.cpp src/main.cpp -o bin/smart_parking.exe
./bin/smart_parking.exe
```

## 4. Resultados da Bateria de Testes
- **100% dos testes de integração aprovados**:
  - Alocação física de vagas (incluindo caminhões ocupando 2 vagas contíguas);
  - Despacho dinâmico de tarifas (Carro, Moto com cilindrada e Caminhão com eixos);
  - Bloqueio por lotação máxima disparando exceção `EstacionamentoLotadoException`;
  - Detecção de placas inexistentes com `TiqueteInvalidoException`;
  - Desalocação determinística sem vazamentos de memória (RAII);
  - Faturamento acumulado e relatórios operacionais consolidados.
