// ============================================================
// ROBÔ CUBO — Phase 3b: IR Remote Control
// ============================================================
// Integrates: IR remote control + HC-SR04 automatic stop
//
// Mapped codes from the ELEGOO remote (verified with decoder):
//   2=Forward  8=Backward  4=Left  6=Right  5=Stop
//   +=Speed+   -=Speed-    CH-=Emergency
// ============================================================

#include <IRremote.hpp>

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

// --- IR Pin ---
#define IR_PIN  2

// --- Safety distances (cm) ---
#define DIST_STOP  20
#define DIST_SLOW  35

// --- Speeds ---
#define SPEED_MAX     220
#define SPEED_MIN     100
#define SPEED_STEP     20
#define SPEED_TURN    110   // fixed, slow speed for turning (doesn't use the adjustable "speed")

// ============================================================
// ELEGOO REMOTE CODES — verified with decoder
// ============================================================
#define BTN_FORWARD     0x18
#define BTN_BACKWARD    0x52
#define BTN_LEFT        0x08
#define BTN_RIGHT       0x5A
#define BTN_STOP        0x1C
#define BTN_SPEED_UP    0x09
#define BTN_SPEED_DOWN  0x07
#define BTN_STOP_ALL    0x45

// ============================================================
// STATE VARIABLES
// ============================================================
int speed = 180;

enum Movement { STOPPED, FORWARD, BACKWARD, LEFT, RIGHT };
Movement currentMovement = STOPPED;
bool obstacleActive = false;

// ============================================================
// HC-SR04 SENSOR
// ============================================================
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 10000); // reduced to 10ms
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
// MAPPING CONFIRMED BY PHYSICAL TESTING:
//   ENA/IN1/IN2 -> RIGHT physical motor
//   ENB/IN3/IN4 -> LEFT physical motor
//
// IN3=HIGH,IN4=LOW  -> Left Motor spins FORWARD
// IN1=HIGH,IN2=LOW  -> Right Motor spins BACKWARD
//                      (so IN1=LOW,IN2=HIGH = FORWARD)
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

void turnRight(int vel) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // Right -> backward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // Left  -> forward
  analogWrite(ENA, vel);   analogWrite(ENB, vel);
}

// ============================================================
// PROCESS IR COMMAND
// ============================================================
void processIR(uint8_t code) {
  switch (code) {

    case BTN_FORWARD:
      currentMovement = FORWARD;
      Serial.print(">> FORWARD  | speed: ");
      Serial.println(speed);
      break;

    case BTN_BACKWARD:
      currentMovement = BACKWARD;
      Serial.print(">> BACKWARD   | speed: ");
      Serial.println(speed);
      break;

    case BTN_LEFT:
      currentMovement = LEFT;
      Serial.print(">> LEFT | fixed speed: ");
      Serial.println(SPEED_TURN);
      break;

    case BTN_RIGHT:
      currentMovement = RIGHT;
      Serial.print(">> RIGHT  | fixed speed: ");
      Serial.println(SPEED_TURN);
      break;

    case BTN_STOP:
    case BTN_STOP_ALL:
      currentMovement = STOPPED;
      Serial.println(">> STOP");
      break;

    case BTN_SPEED_UP:
      speed = min(speed + SPEED_STEP, SPEED_MAX);
      Serial.print(">> SPEED+  -> ");
      Serial.println(speed);
      break;

    case BTN_SPEED_DOWN:
      speed = max(speed - SPEED_STEP, SPEED_MIN);
      Serial.print(">> SPEED-  -> ");
      Serial.println(speed);
      break;

    default:
      Serial.print("   unknown: 0x");
      Serial.println(code, HEX);
      break;
  }
}

// ============================================================
// APPLY MOVEMENT (with sensor safety)
// ============================================================
void applyMovement(float distance) {
  // Safety: block forward motion if obstacle too close
  if (distance < DIST_STOP && currentMovement == FORWARD) {
    stopMotors();
    if (!obstacleActive) {
      Serial.println("!!! OBSTACLE — AUTOMATIC STOP !!!");
      obstacleActive = true;
    }
    return;
  }
  obstacleActive = false;

  switch (currentMovement) {
    case STOPPED:  stopMotors();           break;
    case FORWARD:  goForward(speed);       break;
    case BACKWARD: goBackward(speed);      break;
    case LEFT:     turnLeft(SPEED_TURN);   break;
    case RIGHT:    turnRight(SPEED_TURN);  break;
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
  Serial.println(" Robo Cubo — Phase 3: IR Control");
  Serial.println("================================");
  Serial.println("2=Forward  8=Backward");
  Serial.println("4=Left     6=Right");
  Serial.println("5=Stop     +=Speed+   -=Speed-");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // 1. Read sensor distance
  float distance = readDistance();

  // 2. Check for IR signal
  if (IrReceiver.decode()) {
    uint8_t code  = IrReceiver.decodedIRData.command;
    uint8_t flags = IrReceiver.decodedIRData.flags;
    // Accept signal if it has a valid code and isn't an auto-repeat
    if (code != 0 && !(flags & IRDATA_FLAGS_IS_REPEAT)) {
      processIR(code);
    }
    IrReceiver.resume();
  }

  // 3. Apply movement with sensor safety
  applyMovement(distance);

  delay(20); // reduced from 50ms -> less blocking time
}
