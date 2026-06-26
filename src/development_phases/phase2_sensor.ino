// ============================================================
// ROBÔ CUBO — Phase 2: Ultrasonic Sensor + Automatic Stop
// ============================================================
// Goal: read distance with the HC-SR04 and automatically stop
// the robot as it approaches an obstacle.
//
// You can test the sensor WITHOUT the battery via Serial
// Monitor (USB). To test the robot actually moving, you need
// the battery connected.
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
#define ECHO_PIN   3   // moved from D13 (conflict with IRremote/LED_BUILTIN)

// --- Speeds (0–255) ---
#define SPEED_NORMAL  180
#define SPEED_SLOW    110

// --- Distance zones (cm) ---
#define DIST_STOP  20   // < 20 cm  -> stop completely
#define DIST_SLOW  35   // < 35 cm  -> slow down
                         // >= 35 cm -> normal speed

// ============================================================
// FUNCTION: READ DISTANCE
// ============================================================
float readDistance() {
  // Make sure TRIG starts LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Fire a 10 µs pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration (timeout: 30 ms ≈ 5 meters)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // No echo -> no obstacle detected
  if (duration == 0) return 999.0;

  // Convert to centimeters: duration (µs) / 58
  return duration / 58.0;
}

// ============================================================
// MOVEMENT FUNCTIONS (from Phase 1)
// ============================================================
void stopMotors() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);     analogWrite(ENB, 0);
}

void goForward(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void goBackward(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void turnLeft(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

void turnRight(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
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

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robo Cubo — Phase 2: HC-SR04");
  Serial.println("================================");
  Serial.println("Bring your hand close to the sensor to test!");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  float distance = readDistance();

  // --- Diagnostics via Serial Monitor ---
  Serial.print("Distance: ");
  if (distance >= 999.0) {
    Serial.print("--- cm  |  ");
    Serial.println("CLEAR (no obstacle)");
  } else {
    Serial.print(distance, 1);
    Serial.print(" cm  |  ");

    if (distance < DIST_STOP) {
      Serial.println("STOP  <<< OBSTACLE NEARBY!");
    } else if (distance < DIST_SLOW) {
      Serial.println("SLOW  (warning zone)");
    } else {
      Serial.println("CLEAR  normal speed");
    }
  }

  // --- Motor control ---
  if (distance < DIST_STOP) {
    stopMotors();

  } else if (distance < DIST_SLOW) {
    goForward(SPEED_SLOW);

  } else {
    goForward(SPEED_NORMAL);
  }

  delay(100); // 10 readings per second
}
