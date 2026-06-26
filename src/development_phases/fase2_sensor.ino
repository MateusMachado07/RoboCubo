// ============================================================
// ROBÔ CUBO — Fase 2: Sensor Ultrassónico + Paragem Automática
// ============================================================
// Objetivo: ler a distância com o HC-SR04 e parar o robô
// automaticamente quando se aproxima de um obstáculo.
//
// Podes testar o sensor SEM bateria via Serial Monitor (USB).
// Para testar o robô a mover-se, precisas da bateria.
// ============================================================

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
#define ECHO_PIN   3   // movido de D13 (conflito IRremote/LED_BUILTIN)

// --- Velocidades (0–255) ---
#define VEL_NORMAL  180
#define VEL_LENTA   110

// --- Zonas de distância (cm) ---
#define DIST_PARAR  20   // < 20 cm  → para completamente
#define DIST_LENTA  35   // < 35 cm  → abranda
                         // >= 35 cm → velocidade normal

// ============================================================
// FUNÇÃO: LER DISTÂNCIA
// ============================================================
float lerDistancia() {
  // Garante que TRIG começa LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Dispara pulso de 10 µs
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Mede duração do eco (timeout: 30 ms ≈ 5 metros)
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  // Sem eco → sem obstáculo detetado
  if (duracao == 0) return 999.0;

  // Converte para centímetros: duração (µs) / 58
  return duracao / 58.0;
}

// ============================================================
// FUNÇÕES DE MOVIMENTO (da Fase 1)
// ============================================================
void parar() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);     analogWrite(ENB, 0);
}

void avancar(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void recuar(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void virarEsquerda(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void virarDireita(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  // Motor Esquerdo
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  // Motor Direito
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robô Cubo — Fase 2: HC-SR04");
  Serial.println("================================");
  Serial.println("Aproxima a mao do sensor para testar!");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  float distancia = lerDistancia();

  // --- Diagnóstico via Serial Monitor ---
  Serial.print("Distancia: ");
  if (distancia >= 999.0) {
    Serial.print("--- cm  |  ");
    Serial.println("LIVRE (sem obstaculo)");
  } else {
    Serial.print(distancia, 1);
    Serial.print(" cm  |  ");

    if (distancia < DIST_PARAR) {
      Serial.println("PARAR  <<< OBSTACULO PROXIMO!");
    } else if (distancia < DIST_LENTA) {
      Serial.println("LENTA  (zona de aviso)");
    } else {
      Serial.println("LIVRE  velocidade normal");
    }
  }

  // --- Controlo dos motores ---
  if (distancia < DIST_PARAR) {
    parar();

  } else if (distancia < DIST_LENTA) {
    avancar(VEL_LENTA);

  } else {
    avancar(VEL_NORMAL);
  }

  delay(100); // 10 leituras por segundo
}
