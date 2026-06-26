// ============================================================
// ROBÔ CUBO — Fase 4: Piloto Automático
// ============================================================
// O robô navega autonomamente evitando obstáculos.
//
// Algoritmo (máquina de estados):
//
//   ┌────────────────────────────────────────────────┐
//   │  LIVRE → obstáculo < DIST_PARAR → VIRANDO     │
//   │  VIRANDO: ângulo aleatório 45°-180°,          │
//   │           direção aleatória (esq/dir) → LIVRE │
//   └────────────────────────────────────────────────┘
//
// Sem recuo: ao detetar obstáculo, o robô para,
// vira um ângulo e direção aleatórios, e continua.
//
// Usa millis() em vez de delay() para não bloquear
// a leitura do sensor durante a viragem.
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
#define ECHO_PIN  13

// --- Velocidades ---
#define VEL_NORMAL  180
#define VEL_LENTA   110
#define VEL_VIRAR   110   // velocidade fixa e lenta ao virar

// --- Distâncias (cm) ---
#define DIST_PARAR  35   // aumentado de 20 → deteta obstáculos mais cedo
#define DIST_LENTA  50   // aumentado proporcionalmente

// --- Durações (ms) ---
// Calibração: 350ms ≈ 45°. Assumindo relação linear,
// 180° ≈ 4× esse valor. Ajusta DUR_VIRAR_45 conforme
// a calibração feita anteriormente, que escala os outros.
#define DUR_VIRAR_45    350                  // referência calibrada (45°)
#define DUR_VIRAR_MIN   DUR_VIRAR_45         // mínimo → 45°
#define DUR_VIRAR_MAX   (DUR_VIRAR_45 * 4)   // máximo → 180°

// ============================================================
// MÁQUINA DE ESTADOS
// ============================================================
enum EstadoRobo { LIVRE, VIRANDO };
EstadoRobo estado = LIVRE;

unsigned long tempoInicio    = 0;
int           duracaoVirar   = 0;
bool          virarParaDireita = false;

// ============================================================
// SENSOR HC-SR04
// ============================================================
float lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
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
// MAPEAMENTO REAL CONFIRMADO POR TESTE FÍSICO (igual à Fase 3):
//   ENA/IN1/IN2 → Motor DIREITO físico
//   ENB/IN3/IN4 → Motor ESQUERDO físico
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
// PILOTO AUTOMÁTICO — Máquina de estados não-bloqueante
// ============================================================
void pilotoAutomatico(float distancia) {
  unsigned long agora = millis();

  switch (estado) {

    // ── LIVRE: avança, abranda se necessário ──────────────
    case LIVRE:
      if (distancia < DIST_PARAR) {
        // Obstáculo próximo → para e vira ângulo aleatório (45°-180°)
        parar();
        tempoInicio      = agora;
        virarParaDireita = random(2);                       // 0=esquerda, 1=direita
        duracaoVirar     = random(DUR_VIRAR_MIN, DUR_VIRAR_MAX + 1);

        estado = VIRANDO;
        Serial.print("OBSTACULO → VIRAR ");
        Serial.print(virarParaDireita ? "DIREITA" : "ESQUERDA");
        Serial.print(" (~");
        Serial.print(map(duracaoVirar, DUR_VIRAR_MIN, DUR_VIRAR_MAX, 45, 180));
        Serial.println("°)");

      } else if (distancia < DIST_LENTA) {
        // Zona de aviso → abranda
        avancar(VEL_LENTA);
        Serial.print("AVISO: ");
        Serial.print(distancia, 0);
        Serial.println(" cm → LENTO");

      } else {
        // Caminho livre → velocidade normal
        avancar(VEL_NORMAL);
      }
      break;

    // ── VIRANDO: vira ângulo aleatório e retoma marcha ────
    case VIRANDO:
      if (virarParaDireita) {
        virarDireita(VEL_VIRAR);
      } else {
        virarEsquerda(VEL_VIRAR);
      }

      if (agora - tempoInicio >= duracaoVirar) {
        parar();
        estado = LIVRE;
        Serial.println("LIVRE → AVANCAR");
      }
      break;
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  // Motor E
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  // Motor D
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Seed aleatório usando ruído do pino analógico flutuante
  randomSeed(analogRead(A0));

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robo Cubo — Fase 4: Autopiloto");
  Serial.println("================================");
  Serial.println("A iniciar em 3 segundos...");
  delay(3000);
  Serial.println("GO!");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  float distancia = lerDistancia();
  pilotoAutomatico(distancia);
  delay(50); // 20 ciclos por segundo
}
