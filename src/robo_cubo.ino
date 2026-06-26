// ============================================================
// ROBÔ CUBO — Final Code
// ============================================================
// Two-wheel-drive robot with a caster wheel, controlled by an
// IR remote, with autopilot mode and automatic obstacle
// stop/avoidance via an ultrasonic sensor.
//
// Features:
//   • Manual speed and direction control (IR remote)
//   • Autopilot with random-angle obstacle avoidance (45°-180°)
//   • Toggle Manual <-> Autopilot with remote button 0
//   • Emergency stop (CH- button)
//
// Hardware: Arduino Uno R3 + L298N + 2x TT DC Motor + HC-SR04
//           + IR Receiver (KS0026) + ELEGOO remote
//
// See README.md and docs/wiring.md for the full wiring diagram
// and hardware notes.
// ============================================================

#include <IRremote.hpp>

// --- Motor Pins ---
// Mapping confirmed by physical testing:
//   ENA/IN1/IN2 -> RIGHT Motor
//   ENB/IN3/IN4 -> LEFT Motor
#define ENA  10   // PWM — Right Motor speed
#define IN1  4    // Right Motor direction
#define IN2  5    // Right Motor direction
#define ENB  9    // PWM — Left Motor speed
#define IN3  6    // Left Motor direction
#define IN4  7    // Left Motor direction

// --- HC-SR04 Pins ---
#define TRIG_PIN  12
#define ECHO_PIN  8     // DO NOT use D13 — conflicts with the Arduino's
                        // built-in LED, used internally by IRremote

// --- IR Pin ---
#define IR_PIN  2

// --- Safety Distances (cm) ---
#define DIST_STOP  35    // below this, stop and turn (autopilot mode)
#define DIST_SLOW  50    // below this, slow down (autopilot mode)

// --- Speeds (PWM 0-255) ---
#define SPEED_MAX     220   // safe limit for TT motors at 6-8V
#define SPEED_MIN     100
#define SPEED_STEP     20
#define SPEED_NORMAL  180
#define SPEED_SLOW    110
#define SPEED_TURN    110   // fixed, slow speed used while turning

// --- Autopilot turning durations (ms) ---
// Without wheel encoders, the time<->angle relationship is an
// estimate calibrated by trial. Adjust TURN_DURATION_45 if needed.
#define TURN_DURATION_45    350                       // calibrated reference (45°)
#define TURN_DURATION_MIN   TURN_DURATION_45          // minimum -> 45°
#define TURN_DURATION_MAX   (TURN_DURATION_45 * 4)    // maximum -> 180°

// ============================================================
// ELEGOO REMOTE CODES (NEC protocol, verified with a decoder)
// ============================================================
#define BTN_FORWARD     0x18   // 2
#define BTN_BACKWARD    0x52   // 8
#define BTN_LEFT        0x08   // 4
#define BTN_RIGHT       0x5A   // 6
#define BTN_STOP        0x1C   // 5
#define BTN_SPEED_UP    0x09   // +
#define BTN_SPEED_DOWN  0x07   // -
#define BTN_STOP_ALL    0x45   // CH- (emergency)
#define BTN_AUTOPILOT   0x16   // 0   (toggle mode)

// ============================================================
// GLOBAL STATE
// ============================================================
enum Mode { MANUAL, AUTOPILOT };
Mode currentMode = MANUAL;

int speed = 180;
enum Movement { STOPPED, FORWARD, BACKWARD, LEFT, RIGHT };
Movement currentMovement = STOPPED;

enum AutoState { CLEAR, TURNING };
AutoState autoState = CLEAR;
unsigned long autoStartTime = 0;
int turnDuration = 0;
bool turnRight = false;

// ============================================================
// HC-SR04 SENSOR
// ============================================================
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 10000);
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

void turnLeftFn(int vel) {
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
// PROCESS IR COMMAND
// ============================================================
void processIR(uint8_t code) {

  // Mode toggle works at any time
  if (code == BTN_AUTOPILOT) {
    stopMotors();
    if (currentMode == MANUAL) {
      currentMode = AUTOPILOT;
      autoState = CLEAR;
      Serial.println("\n>>> MODE: AUTOPILOT <<<\n");
    } else {
      currentMode = MANUAL;
      currentMovement = STOPPED;
      Serial.println("\n>>> MODE: MANUAL <<<\n");
    }
    return;
  }

  // Emergency stop works in any mode
  if (code == BTN_STOP_ALL) {
    stopMotors();
    currentMovement = STOPPED;
    currentMode = MANUAL;
    Serial.println(">> EMERGENCY STOP — switched to MANUAL");
    return;
  }

  // Remaining buttons only work in manual mode
  if (currentMode != MANUAL) {
    Serial.println("   (ignored — robot is in AUTOPILOT mode)");
    return;
  }

  switch (code) {
    case BTN_FORWARD:
      currentMovement = FORWARD;
      Serial.print(">> FORWARD  | speed: ");
      Serial.println(speed);
      break;

    case BTN_BACKWARD:
      currentMovement = BACKWARD;
      Serial.print(">> BACKWARD | speed: ");
      Serial.println(speed);
      break;

    case BTN_LEFT:
      currentMovement = LEFT;
      Serial.print(">> LEFT     | fixed speed: ");
      Serial.println(SPEED_TURN);
      break;

    case BTN_RIGHT:
      currentMovement = RIGHT;
      Serial.print(">> RIGHT    | fixed speed: ");
      Serial.println(SPEED_TURN);
      break;

    case BTN_STOP:
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
      Serial.print("   unknown code: 0x");
      Serial.println(code, HEX);
      break;
  }
}

// ============================================================
// MANUAL MODE — direct movement, no automatic stop
// ============================================================
void manualMode(float distance) {
  switch (currentMovement) {
    case STOPPED:  stopMotors();              break;
    case FORWARD:  goForward(speed);          break;
    case BACKWARD: goBackward(speed);         break;
    case LEFT:     turnLeftFn(SPEED_TURN);    break;
    case RIGHT:    turnRightFn(SPEED_TURN);   break;
  }
}

// ============================================================
// AUTOPILOT MODE — non-blocking state machine
// ============================================================
void autopilotMode(float distance) {
  unsigned long now = millis();

  switch (autoState) {

    case CLEAR:
      if (distance < DIST_STOP) {
        stopMotors();
        autoStartTime = now;
        turnRight     = random(2);
        turnDuration  = random(TURN_DURATION_MIN, TURN_DURATION_MAX + 1);

        autoState = TURNING;
        Serial.print("OBSTACLE -> TURNING ");
        Serial.print(turnRight ? "RIGHT" : "LEFT");
        Serial.print(" (~");
        Serial.print(map(turnDuration, TURN_DURATION_MIN, TURN_DURATION_MAX, 45, 180));
        Serial.println(" deg)");

      } else if (distance < DIST_SLOW) {
        goForward(SPEED_SLOW);

      } else {
        goForward(SPEED_NORMAL);
      }
      break;

    case TURNING:
      if (turnRight) {
        turnRightFn(SPEED_TURN);
      } else {
        turnLeftFn(SPEED_TURN);
      }

      if (now - autoStartTime >= turnDuration) {
        stopMotors();
        autoState = CLEAR;
        Serial.println("CLEAR -> FORWARD");
      }
      break;
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
  randomSeed(analogRead(A0));

  Serial.begin(9600);
  Serial.println("================================");
  Serial.println(" ROBO CUBO — Final Code");
  Serial.println("================================");
  Serial.println("2=Forward   8=Backward");
  Serial.println("4=Left      6=Right");
  Serial.println("5=Stop      +=Speed+   -=Speed-");
  Serial.println("0=Toggle Manual/Autopilot");
  Serial.println("CH-=Emergency stop");
  Serial.println();
  Serial.println(">>> INITIAL MODE: MANUAL <<<");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  float distance = readDistance();

  if (IrReceiver.decode()) {
    uint8_t code  = IrReceiver.decodedIRData.command;
    uint8_t flags = IrReceiver.decodedIRData.flags;
    if (code != 0 && !(flags & IRDATA_FLAGS_IS_REPEAT)) {
      processIR(code);
    }
    IrReceiver.resume();
  }

  if (currentMode == MANUAL) {
    manualMode(distance);
  } else {
    autopilotMode(distance);
  }

  delay(20);
}
