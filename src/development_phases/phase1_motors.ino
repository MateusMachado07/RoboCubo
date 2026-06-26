// ============================================================
// ROBÔ CUBO — Phase 1: Motor Test
// ============================================================
// Goal: confirm the motors and L298N are wired correctly by
// testing every movement.
//
// Open the Serial Monitor (9600 baud) to see status.
// ============================================================

// --- Right Motor Pins ---
#define ENA  10   // PWM — Right Motor speed
#define IN1  4
#define IN2  5

// --- Left Motor Pins ---
#define ENB  9    // PWM — Left Motor speed
#define IN3  6
#define IN4  7

// --- Speeds (0 to 255) ---
#define SPEED_NORMAL  180
#define SPEED_SLOW    110

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" Robo Cubo — Phase 1: Motors");
  Serial.println("================================");
  Serial.println("Starting in 2 seconds...");
  delay(2000);
}

void stopMotors() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);     analogWrite(ENB, 0);
  Serial.println(">> STOP");
}

void goForward(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
  Serial.print(">> FORWARD | speed: ");
  Serial.println(vel);
}

void goBackward(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
  Serial.print(">> BACKWARD | speed: ");
  Serial.println(vel);
}

void turnLeft(int vel) {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
  Serial.print(">> LEFT | speed: ");
  Serial.println(vel);
}

void turnRight(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
  Serial.print(">> RIGHT | speed: ");
  Serial.println(vel);
}

void loop() {
  Serial.println("\n--- New test sequence ---");

  goForward(SPEED_NORMAL);
  delay(2000);

  stopMotors();
  delay(1000);

  goBackward(SPEED_NORMAL);
  delay(2000);

  stopMotors();
  delay(1000);

  turnLeft(SPEED_NORMAL);
  delay(1500);

  stopMotors();
  delay(1000);

  turnRight(SPEED_NORMAL);
  delay(1500);

  stopMotors();
  delay(3000); // pause before repeating
}
