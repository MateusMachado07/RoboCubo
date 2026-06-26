// ============================================================
// ROBÔ CUBO — Fase 3b: Controlo por Telecomando IR
// ============================================================
// Integra: controlo remoto IR + paragem automática HC-SR04
//
// Códigos mapeados do comando ELEGOO (verificados com decoder):
//   2=Avançar  8=Recuar  4=Esq  6=Dir  5=Parar
//   +=Vel+     -=Vel-    CH-=Emergência
// ============================================================

#include <IRremote.hpp>

// --- Pinos Motor Esquerdo ---
#define ENA  10
#define IN1  4
#define IN2  5

// --- Pinos Motor Direito ---
#define ENB  9
#define IN3  6
#define IN4  7

// --- Pinos HC-SR04 ---
#define TRIG_PIN  12
#define ECHO_PIN  13

// --- Pino IR ---
#define IR_PIN  2

// --- Distâncias de segurança (cm) ---
#define DIST_PARAR  20
#define DIST_LENTA  35

// --- Velocidades ---
#define VEL_MAX    220
#define VEL_MIN    100
#define VEL_PASSO   20
#define VEL_VIRAR  110   // velocidade lenta e fixa para virar (não usa "velocidade" ajustável)

// ============================================================
// CÓDIGOS DO COMANDO ELEGOO — verificados com decoder
// ============================================================
#define BTN_AVANCAR   0x18
#define BTN_RECUAR    0x52
#define BTN_ESQUERDA  0x08
#define BTN_DIREITA   0x5A
#define BTN_PARAR     0x1C
#define BTN_VEL_UP    0x09
#define BTN_VEL_DOWN  0x07
#define BTN_STOP_ALL  0x45

// ============================================================
// VARIÁVEIS DE ESTADO
// ============================================================
int velocidade = 180;

enum Movimento { PARADO, AVANCANDO, RECUANDO, ESQUERDA, DIREITA };
Movimento movimentoAtual = PARADO;
bool obstaculoAtivo = false;

// ============================================================
// SENSOR HC-SR04
// ============================================================
float lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracao = pulseIn(ECHO_PIN, HIGH, 10000); // reduzido para 10ms
  if (duracao == 0) return 999.0;
  return duracao / 58.0;
}

// ============================================================
// MOTORES
// ============================================================
void parar() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);     analogWrite(ENB, 0);
}

// ============================================================
// MAPEAMENTO REAL CONFIRMADO POR TESTE FÍSICO:
//   ENA/IN1/IN2 → Motor DIREITO físico
//   ENB/IN3/IN4 → Motor ESQUERDO físico
//
// IN3=HIGH,IN4=LOW  → Motor Esquerdo gira para a FRENTE
// IN1=HIGH,IN2=LOW  → Motor Direito gira para TRÁS
//                      (logo IN1=LOW,IN2=HIGH = FRENTE)
// ============================================================

void avancar(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Direito → frente
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Esquerdo → frente
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void recuar(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Direito → trás
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Esquerdo → trás
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void virarEsquerda(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Direito → frente
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Esquerdo → trás
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void virarDireita(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Direito → trás
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Esquerdo → frente
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

// ============================================================
// PROCESSAR COMANDO IR
// ============================================================
void processarIR(uint8_t codigo) {
  switch (codigo) {

    case BTN_AVANCAR:
      movimentoAtual = AVANCANDO;
      Serial.print(">> AVANCAR  | vel: ");
      Serial.println(velocidade);
      break;

    case BTN_RECUAR:
      movimentoAtual = RECUANDO;
      Serial.print(">> RECUAR   | vel: ");
      Serial.println(velocidade);
      break;

    case BTN_ESQUERDA:
      movimentoAtual = ESQUERDA;
      Serial.print(">> ESQUERDA | vel fixa: ");
      Serial.println(VEL_VIRAR);
      break;

    case BTN_DIREITA:
      movimentoAtual = DIREITA;
      Serial.print(">> DIREITA  | vel fixa: ");
      Serial.println(VEL_VIRAR);
      break;

    case BTN_PARAR:
    case BTN_STOP_ALL:
      movimentoAtual = PARADO;
      Serial.println(">> PARAR");
      break;

    case BTN_VEL_UP:
      velocidade = min(velocidade + VEL_PASSO, VEL_MAX);
      Serial.print(">> VEL+  → ");
      Serial.println(velocidade);
      break;

    case BTN_VEL_DOWN:
      velocidade = max(velocidade - VEL_PASSO, VEL_MIN);
      Serial.print(">> VEL-  → ");
      Serial.println(velocidade);
      break;

    default:
      Serial.print("   desconhecido: 0x");
      Serial.println(codigo, HEX);
      break;
  }
}

// ============================================================
// APLICAR MOVIMENTO (com segurança do sensor)
// ============================================================
void aplicarMovimento(float distancia) {
  // Segurança: bloqueia avanço se obstáculo muito próximo
  if (distancia < DIST_PARAR && movimentoAtual == AVANCANDO) {
    parar();
    if (!obstaculoAtivo) {
      Serial.println("!!! OBSTACULO — PARAGEM AUTOMATICA !!!");
      obstaculoAtivo = true;
    }
    return;
  }
  obstaculoAtivo = false;

  switch (movimentoAtual) {
    case PARADO:    parar();                    break;
    case AVANCANDO: avancar(velocidade);        break;
    case RECUANDO:  recuar(velocidade);         break;
    case ESQUERDA:  virarEsquerda(VEL_VIRAR);  break;
    case DIREITA:   virarDireita(VEL_VIRAR);   break;
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robo Cubo — Fase 3: IR Control");
  Serial.println("================================");
  Serial.println("2=Avancar  8=Recuar");
  Serial.println("4=Esq      6=Dir");
  Serial.println("5=Parar    +=Vel+   -=Vel-");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // 1. Ler distância do sensor
  float distancia = lerDistancia();

  // 2. Verificar sinal IR
  if (IrReceiver.decode()) {
    uint8_t codigo = IrReceiver.decodedIRData.command;
    uint8_t flags  = IrReceiver.decodedIRData.flags;
    // Aceita sinal se tem código válido e não é repetição automática
    if (codigo != 0 && !(flags & IRDATA_FLAGS_IS_REPEAT)) {
      processarIR(codigo);
    }
    IrReceiver.resume();
  }

  // 3. Aplicar movimento com sensor de segurança
  aplicarMovimento(distancia);

  delay(20); // reduzido de 50ms → menos tempo bloqueado
}
