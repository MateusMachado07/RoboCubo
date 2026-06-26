// ============================================================
// ROBÔ CUBO — Phase 4: Autopilot
// ============================================================
// The robot drives autonomously, avoiding obstacles.
//
// Algorithm (state machine):
//
//   ┌────────────────────────────────────────────────┐
//   │  CLEAR -> obstacle < DIST_STOP -> TURNING     │
//   │  TURNING: random angle 45°-180°,              │
//   │           random direction (left/right) -> CLEAR │
//   └────────────────────────────────────────────────┘
//
// No reversing: on detecting an obstacle, the robot stops,
// turns a random angle and direction, then continues.
//
// Uses millis() instead of delay() so the sensor keeps being
// read during the turn.
// ============================================================

// --- Left Motor Pins ---
#define ENA  10
#define IN1  4
#define IN2  5

// --- Right Motor Pins ---
#define ENB  9
#define IN3  6
#define IN4  7

// --- HC-SR04 Pins ---
#define TRIG_PIN  12
#define ECHO_PIN  13

// --- Speeds ---
#define SPEED_NORMAL  180
#define SPEED_SLOW    110
#define SPEED_TURN    110   // fixed, slow speed while turning

// --- Distances (cm) ---
#define DIST_STOP  35   // increased from 20 -> detects obstacles earlier
#define DIST_SLOW  50   // increased proportionally

// --- Durations (ms) ---
// Calibration: 350ms ≈ 45°. Assuming a linear relationship,
// 180° ≈ 4x that value. Adjust TURN_DURATION_45 according to
// your own calibration; the others scale automatically.
#define TURN_DURATION_45    350                       // calibrated reference (45°)
#define TURN_DURATION_MIN   TURN_DURATION_45          // minimum -> 45°
#define TURN_DURATION_MAX   (TURN_DURATION_45 * 4)    // maximum -> 180°

// ============================================================
// STATE MACHINE
// ============================================================
enum RobotState { CLEAR, TURNING };
RobotState state = CLEAR;

unsigned long startTime    = 0;
int           turnDuration = 0;
bool          turnRight    = false;

// ============================================================
// HC-SR04 SENSOR
// ============================================================
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999.0;
  return duration / 58.0;
}

// ============================================================
// MOTORS
// ============================================================
void stopMotors() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);     analogWrite(ENB, 0);
}

// ============================================================
// MAPPING CONFIRMED BY PHYSICAL TESTING (same as Phase 3):
//   ENA/IN1/IN2 -> RIGHT physical motor
//   ENB/IN3/IN4 -> LEFT physical motor
// ============================================================

void goForward(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Right -> forward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Left  -> forward
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void goBackward(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Right -> backward
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Left  -> backward
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void turnLeft(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // Right -> forward
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // Left  -> backward
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void turnRightFn(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Right -> backward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Left  -> forward
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

// ============================================================
// AUTOPILOT — Non-blocking state machine
// ============================================================
void autopilot(float distance) {
  unsigned long now = millis();

  switch (state) {

    // ── CLEAR: go forward, slow down if needed ────────────
    case CLEAR:
      if (distance < DIST_STOP) {
        // Obstacle nearby -> stop and turn a random angle (45°-180°)
        stopMotors();
        startTime    = now;
        turnRight    = random(2);                       // 0=left, 1=right
        turnDuration = random(TURN_DURATION_MIN, TURN_DURATION_MAX + 1);

        state = TURNING;
        Serial.print("OBSTACLE -> TURNING ");
        Serial.print(turnRight ? "RIGHT" : "LEFT");
        Serial.print(" (~");
        Serial.print(map(turnDuration, TURN_DURATION_MIN, TURN_DURATION_MAX, 45, 180));
        Serial.println(" deg)");

      } else if (distance < DIST_SLOW) {
        // Warning zone -> slow down
        goForward(SPEED_SLOW);
        Serial.print("WARNING: ");
        Serial.print(distance, 0);
        Serial.println(" cm -> SLOW");

      } else {
        // Clear path -> normal speed
        goForward(SPEED_NORMAL);
      }
      break;

    // ── TURNING: turns a random angle, then resumes ───────
    case TURNING:
      if (turnRight) {
        turnRightFn(SPEED_TURN);
      } else {
        turnLeft(SPEED_TURN);
      }

      if (now - startTime >= turnDuration) {
        stopMotors();
        state = CLEAR;
        Serial.println("CLEAR -> FORWARD");
      }
      break;
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  // Left Motor
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  // Right Motor
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Random seed using noise from a floating analog pin
  randomSeed(analogRead(A0));

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robo Cubo — Phase 4: Autopilot");
  Serial.println("================================");
  Serial.println("Starting in 3 seconds...");
  delay(3000);
  Serial.println("GO!");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  float distance = readDistance();
  autopilot(distance);
  delay(50); // 20 cycles per second
}
