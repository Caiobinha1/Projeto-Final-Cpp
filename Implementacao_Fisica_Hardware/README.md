# Guia Prático de Implementação Física do Smart Parking (Hardware)
**Universidade Federal de Santa Catarina (UFSC) – Centro Tecnológico (CTC)**  
**Departamento de Engenharia Elétrica e Eletrônica (EEL)**  
**Disciplina:** EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados  
**Docentes:** Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler  

---

## 1. É Obrigatório Montar o Circuito Físico?

### Resposta Direta: **NÃO é obrigatório para ser aprovado!**
Conforme a especificação oficial do Prof. Dr. Eduardo Augusto Bezerra (mantida na página da disciplina e em projetos anteriores):

> *"Demonstração do programa funcionando em um microcomputador **OU** em um sistema embarcado (ou em ambos)."*

Isso significa que:
* **Apenas Simulação no Computador:** É **100% suficiente** para cumprir todos os requisitos da disciplina. O software compilado em C++ (`smart_parking.exe`) e o **Simulador Visual Gráfico (`simulador_visual.py`)** atendem integralmente à entrega final;
* **Por que este guia foi criado?** O Prof. Bezerra é especialista em hardware embarcado (Laboratório GSE / Satélites FloripaSat). Apresentar uma pequena maquete física com a cancela abrindo de verdade impressiona a banca, garante nota 10 com louvor e destaca o projeto perante a turma. Mesmo que vocês optem por não montar fisicamente, anexar este projeto de hardware no relatório demonstra maturidade completa de engenharia!

---

## 2. Lista de Componentes Baratos e de Fácil Acesso no Brasil

Todos os componentes abaixo são padrão de mercado, custam poucos reais e são encontrados para entrega imediata no Mercado Livre, Shopee ou em lojas de eletrônica locais:

| Componente | Função no Sistema | Preço Médio no Brasil | Onde Encontrar |
| :--- | :--- | :---: | :--- |
| **Arduino Nano ou Uno (com cabo USB)** | Microcontrolador embarcado que executa o firmware e lê sensores | R$ 25,00 – R$ 38,00 | Mercado Livre / Shopee |
| **Micro Servo Motor SG90 (9g)** | Atuador mecânico da cancela (levanta a haste a 90° e fecha a 0°) | R$ 10,00 – R$ 14,00 | Lojas de robótica / ML |
| **Sensor Ultrassônico HC-SR04** | Sensor de presença de entrada (detecta veículo a menos de 10 cm) | R$ 9,00 – R$ 12,00 | Qualquer loja maker |
| **Módulo Sensor de Obstáculo IR** | Sensor óptico de travessia (detecta quando o veículo cruzou o vão) | R$ 5,00 – R$ 7,00 | Mercado Livre / Shopee |
| **Botão Push-Button Táctil (Chave 6x6mm)** | Botão físico do totem para o condutor solicitar o tíquete | R$ 0,50 – R$ 1,50 | Lojas de eletrônica |
| **Display LCD 16x2 com Módulo I2C** | Tela do totem (exibe "Aproxime Veículo", "Vagas Livres", "Tíquete #") | R$ 18,00 – R$ 24,00 | ML / Shopee |
| **1x LED Difuso Verde + 1x LED Vermelho (5mm)** | Semáforo visual da cancela (Verde = Siga / Vermelho = Pare) | R$ 0,50 | Qualquer loja |
| **2x Resistores de 220Ω ou 330Ω (1/4W)** | Limitadores de corrente para os LEDs do semáforo | R$ 0,20 | Qualquer loja |
| **Buzzer Ativo 5V** | Sinalizador sonoro de confirmação e alerta de timeout | R$ 2,50 – R$ 4,00 | Lojas de eletrônica |
| **Mini Protoboard 400 Furos + Fios Jumpers** | Conexões sem necessidade de solda | R$ 15,00 – R$ 18,00 | Kits de prototipagem |

> **Custo Total Estimado do Kit Completo:** Entre **R$ 65,00 e R$ 85,00** (dividido pela dupla, sai por menos de R$ 45,00 por aluno).

---

## 3. Tabela de Ligação Pino a Pino (Wiring Pinout)

Basta conectar os fios na protoboard seguindo rigorosamente a tabela abaixo:

```
[ Arduino Nano / Uno ]
 ├── D2  ------------> Ânodo do LED Vermelho (com resistor de 220Ω em série para o GND)
 ├── D3  ------------> Ânodo do LED Verde (com resistor de 220Ω em série para o GND)
 ├── D4  ------------> Um terminal do Push-Button (o outro terminal vai direto ao GND)
 ├── D5  ------------> Pino OUT do Sensor de Obstáculo IR (Travessia)
 ├── D6  ------------> Pino ECHO do Sensor Ultrassônico HC-SR04
 ├── D7  ------------> Pino TRIGGER do Sensor Ultrassônico HC-SR04
 ├── D8  ------------> Terminal (+) do Buzzer Ativo (terminal negativo no GND)
 ├── D9  (PWM) ------> Fio Laranja (Sinal) do Servo Motor SG90
 ├── A4  (SDA) ------> Pino SDA do Display LCD I2C
 ├── A5  (SCL) ------> Pino SCL do Display LCD I2C
 ├── 5V  ------------> Barramento Vermelho (+) da Protoboard (Alimenta VCC de todos os módulos)
 └── GND ------------> Barramento Azul (-) da Protoboard (GND comum de todos os componentes)
```

> **Dica Elétrica:** O Servo SG90 consome pouca corrente (cerca de 150mA em movimento), podendo ser alimentado diretamente pelos 5V da porta USB do Arduino. Para o botão, o código utiliza `pinMode(PINO_BOTAO_TOTEM, INPUT_PULLUP)`, dispensando resistores externos adicionais!

---

## 4. Construção da Maquete Mecânica da Cancela

Para criar uma maquete didática e profissional em menos de 30 minutos:
1. **Base da Cancela:** Cole uma caixinha de papelão pequena (ou um suporte de madeira/plástico) sobre uma base de papelão representando o asfalto;
2. **Fixação do Micro Servo:** Fixe o servo motor SG90 verticalmente na lateral da caixinha com fita dupla face de espuma ou cola quente;
3. **Haste da Cancela:** Pegue a cruzeta/braço de plástico que já vem na caixinha do servo SG90 e cole um **palito de picolé** ou **canudo plástico** com listras adesivas vermelhas e brancas (feitas com fita isolante ou caneta piloto);
4. **Posicionamento dos Sensores:**
   - Posicione o Sensor Ultrassônico virado para a frente da cancela (onde os carrinhos de brinquedo vão parar);
   - Posicione o Sensor Infravermelho logo após a haste da cancela (para detectar quando a traseira do carrinho cruzar o vão).

---

## 5. Como Gravar o Firmware no Microcontrolador

1. Baixe e instale a **[Arduino IDE](https://www.arduino.cc/en/software)** (gratuita);
2. Conecte o Arduino na porta USB do computador;
3. No menu *Ferramentas -> Gerenciador de Bibliotecas*, busque e instale a biblioteca:
   - **`LiquidCrystal I2C`** (por Frank de Brabander ou Marco Schwartz);
4. Abra o arquivo [`firmware_arduino_cancela.ino`](./firmware_arduino_cancela.ino);
5. Selecione a placa (*Arduino Uno* ou *Arduino Nano*) e a porta COM correspondente;
6. Clique no botão **Carregar (Upload - seta para a direita)**.

Pronto! Assim que o código subir, o LCD acenderá exibindo *"SMART PARKING - UFSC"*, a cancela fará um autoteste fechando na horizontal e o LED vermelho acenderá!

---

## 6. Integração Opcional USB-Serial com o Motor C++ do PC

O firmware já inclui um protocolo serial bidirecional em 9600 baud. Se vocês quiserem conectar a maquete física com o software C++ no computador:
- O Arduino envia eventos pela USB (`EVT_VEICULO_CHEGOU`, `EVT_TIQUETE_RETIRADO_ABRIR`, `EVT_PASSAGEM_CONCLUIDA`);
- O motor C++ (`smart_parking.exe`) processa a FSM, aloca a vaga polimórfica e envia de volta comandos como `CMD_ABRIR` ou `CMD_LCD VAGA 01 ALOCADA`.

Esse ecossistema unificado prova aos professores a robustez da arquitetura em camadas projetada para a disciplina!
