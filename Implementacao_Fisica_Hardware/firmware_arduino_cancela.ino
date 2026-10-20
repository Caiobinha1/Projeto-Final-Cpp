/**
 * ==============================================================================
 * UFSC - Centro Tecnologico (CTC) - Depto. de Engenharia Eletrica e Eletronica (EEL)
 * Disciplina: EEL7323 / EEL510265 - Programacao C++ para Sistemas Embarcados
 * Docentes: Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler
 * 
 * PROJETO FINAL: SMART PARKING CONTROLLER
 * FIRMWARE DE HARDWARE: CONTROLADOR DE CANCELA E SENSORES FISICOS
 * Placa Alvo: Arduino Uno / Nano (ATmega328P) ou ESP32
 * ==============================================================================
 */

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Biblioteca: LiquidCrystal I2C

// --- MAPEAMENTO DE PINOS (PINOUT DE BAIXO CUSTO) ---
const int PINO_SERVO_CANCELA   = 9;  // PWM para Micro Servo SG90
const int PINO_LED_VERMELHO    = 2;  // Semáforo: Pare / Cancela Fechada
const int PINO_LED_VERDE       = 3;  // Semáforo: Siga / Cancela Aberta
const int PINO_BOTAO_TOTEM     = 4;  // Botão táctil emissor de tíquete (INPUT_PULLUP)
const int PINO_SENSOR_PASSAGEM = 5;  // Módulo Obstáculo IR (detecção de travessia)
const int PINO_US_ECHO         = 6;  // Sensor Ultrassônico HC-SR04: Pino Echo
const int PINO_US_TRIGGER      = 7;  // Sensor Ultrassônico HC-SR04: Pino Trigger
const int PINO_BUZZER          = 8;  // Buzzer ativo 5V para alertas sonoros

// --- OBJETOS DE HARDWARE ---
Servo servoCancela;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço padrão 0x27 (16 colunas x 2 linhas)

// --- ESTADOS DA FSM LOCAL ---
enum EstadoHardware {
    HW_LIVRE,
    HW_VEICULO_PRESENTE,
    HW_EMITINDO_TIQUETE,
    HW_CANCELA_ABERTA,
    HW_ALERTA_TIMEOUT
};

EstadoHardware estadoFsm = HW_LIVRE;
unsigned long tempoAbertura = 0;
const unsigned long TIMEOUT_CANCELA_MS = 10000; // 10 segundos para passar

// --- FUNÇÃO PARA MEDIR DISTÂNCIA COM HC-SR04 (cm) ---
float medirDistanciaUltrassom() {
    digitalWrite(PINO_US_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(PINO_US_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PINO_US_TRIGGER, LOW);

    long duracaoPulso = pulseIn(PINO_US_ECHO, HIGH, 25000); // Timeout 25ms
    if (duracaoPulso == 0) return 999.0; // Sem eco / fora de alcance

    float distanciaCm = (duracaoPulso * 0.0343) / 2.0;
    return distanciaCm;
}

// --- CONTROLE DA CANCELA FÍSICA ---
void abrirCancela() {
    servoCancela.write(90); // Cancela levantada a 90 graus
    digitalWrite(PINO_LED_VERMELHO, LOW);
    digitalWrite(PINO_LED_VERDE, HIGH);

    // Beep curto de liberação
    tone(PINO_BUZZER, 2000, 150);
}

void fecharCancela() {
    servoCancela.write(0);  // Cancela na horizontal (bloqueada)
    digitalWrite(PINO_LED_VERDE, LOW);
    digitalWrite(PINO_LED_VERMELHO, HIGH);
}

void setup() {
    // Inicialização da porta serial para ponte com o computador
    Serial.begin(9600);

    // Configuração dos pinos digitais
    pinMode(PINO_LED_VERMELHO, OUTPUT);
    pinMode(PINO_LED_VERDE, OUTPUT);
    pinMode(PINO_BUZZER, OUTPUT);
    pinMode(PINO_US_TRIGGER, OUTPUT);
    pinMode(PINO_US_ECHO, INPUT);
    pinMode(PINO_BOTAO_TOTEM, INPUT_PULLUP);
    pinMode(PINO_SENSOR_PASSAGEM, INPUT);

    // Inicialização do Servo
    servoCancela.attach(PINO_SERVO_CANCELA);
    fecharCancela();

    // Inicialização do LCD I2C
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SMART PARKING");
    lcd.setCursor(0, 1);
    lcd.print("UFSC - EEL7323");
    delay(1500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CANCELA PRONTA");
    lcd.setCursor(0, 1);
    lcd.print("VAGAS: 10 LIVRES");

    Serial.println("ARDUINO_SMART_PARKING_ONLINE");
}

void loop() {
    // 1. Leitura dos Sensores Físicos
    float distanciaEntrada = medirDistanciaUltrassom();
    bool veiculoNaFrente = (distanciaEntrada < 12.0 && distanciaEntrada > 1.0); // Menos de 12 cm
    bool botaoPressionado = (digitalRead(PINO_BOTAO_TOTEM) == LOW); // Botão pressionado
    bool sensorPassagemLivre = (digitalRead(PINO_SENSOR_PASSAGEM) == HIGH); // IR desobstruído

    // 2. Máquina de Estados do Hardware
    switch (estadoFsm) {
        case HW_LIVRE:
            fecharCancela();
            if (veiculoNaFrente) {
                estadoFsm = HW_VEICULO_PRESENTE;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("VEICULO DETECT.");
                lcd.setCursor(0, 1);
                lcd.print("PRESSIONE BOTAO");
                Serial.println("EVT_VEICULO_CHEGOU");
            }
            break;

        case HW_VEICULO_PRESENTE:
            if (!veiculoNaFrente) {
                // Veículo recuou antes de emitir tíquete
                estadoFsm = HW_LIVRE;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("DESISTENCIA");
                delay(1000);
                lcd.clear();
                lcd.print("CANCELA LIVRE");
                Serial.println("EVT_VEICULO_RECUOU");
            } else if (botaoPressionado) {
                estadoFsm = HW_EMITINDO_TIQUETE;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("EMITINDO TIQUETE");
                lcd.setCursor(0, 1);
                lcd.print("AGUARDE RETIRADA");
                tone(PINO_BUZZER, 1500, 300);
                delay(800); // Simula impressão

                // Abre a cancela
                abrirCancela();
                tempoAbertura = millis();
                estadoFsm = HW_CANCELA_ABERTA;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("CANCELA ABERTA");
                lcd.setCursor(0, 1);
                lcd.print("BOA VIAGEM!");
                Serial.println("EVT_TIQUETE_RETIRADO_ABRIR");
            }
            break;

        case HW_CANCELA_ABERTA:
            // Aguarda o veículo atravessar e cruzar o sensor IR
            if (digitalRead(PINO_SENSOR_PASSAGEM) == LOW) {
                // Carro está no vão da cancela
                tempoAbertura = millis(); // Reinicia timeout enquanto há carro no meio
            } else if (millis() - tempoAbertura > 4000 && !veiculoNaFrente) {
                // Passagem concluída com sucesso (4s após liberar vão e entrada)
                fecharCancela();
                estadoFsm = HW_LIVRE;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("PASSAGEM OK!");
                delay(1000);
                lcd.clear();
                lcd.print("CANCELA LIVRE");
                Serial.println("EVT_PASSAGEM_CONCLUIDA");
            } else if (millis() - tempoAbertura > TIMEOUT_CANCELA_MS) {
                // Expirou o tempo limite de segurança
                estadoFsm = HW_ALERTA_TIMEOUT;
                digitalWrite(PINO_LED_VERDE, LOW);
                digitalWrite(PINO_LED_VERMELHO, HIGH);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("ALERTA TIMEOUT!");
                lcd.setCursor(0, 1);
                lcd.print("DESOBSTRUA VAO");
                Serial.println("EVT_TIMEOUT_ALERTA");
            }
            break;

        case HW_ALERTA_TIMEOUT:
            // Apita buzzer intermitente
            tone(PINO_BUZZER, 800, 100);
            delay(200);
            if (sensorPassagemLivre && !veiculoNaFrente) {
                // Desobstruído: fecha e reseta
                fecharCancela();
                estadoFsm = HW_LIVRE;
                lcd.clear();
                lcd.print("SISTEMA RESTAUR.");
                delay(1000);
                lcd.clear();
                lcd.print("CANCELA LIVRE");
                Serial.println("EVT_RESET_SISTEMA");
            }
            break;
    }

    // 3. Processamento de Comandos Externos vindos do PC via USB Serial
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "CMD_ABRIR") {
            abrirCancela();
        } else if (cmd == "CMD_FECHAR") {
            fecharCancela();
        } else if (cmd.startsWith("CMD_LCD ")) {
            String texto = cmd.substring(8);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(texto);
        }
    }

    delay(50); // Taxa de amostragem de 20 Hz
}
