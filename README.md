# 🤖 Robô Cubo (Cube Robot)

A cube-shaped autonomous robot built with an Arduino Uno R3, controlled by an IR remote, with an autopilot mode for obstacle avoidance.

![Status](https://img.shields.io/badge/status-completed-success)
![Platform](https://img.shields.io/badge/platform-Arduino_Uno_R3-00979D)
![License](https://img.shields.io/badge/license-MIT-blue)

---

## 📋 About the Project

Robô Cubo is a two-wheeled robot with a rear caster (ball) wheel for balance, capable of:

- 🎮 Being controlled manually via an infrared remote
- 🚀 Navigating fully autonomously, avoiding obstacles
- 📡 Detecting obstacles with an ultrasonic sensor
- 🔄 Switching between the two modes with a single button press

This project was built from scratch, with about €26 spent on new components, reusing the Arduino and sensors from an existing ELEGOO Super Starter kit.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Manual control** | Speed and direction via IR remote, with independent PWM control on each motor |
| **Autopilot** | Drives on its own; on detecting an obstacle, it stops and turns a random angle (45°–180°) in a random direction, then continues |
| **Emergency stop** | Dedicated remote button to stop everything instantly, in any mode |
| **Mode switching** | A single button (0) toggles autopilot on/off |

> ℹ️ Automatic obstacle stopping is active **only in autopilot mode** — in manual mode, the driver has full control (a deliberate design choice).

---

## 🧰 Hardware Used

| Component | Model / Spec | Notes |
|---|---|---|
| Microcontroller | Arduino Uno R3 | |
| Motor Driver | L298N (ST chip), Keyestudio KS0063 module | 2nd unit — the first one burned out during development |
| Motors | 2× TT DC Gear Motor, 1:48 ratio, 3-6V | Replaced the originally planned 28BYJ-48 steppers |
| Wheels | 2× drive wheels + 1 caster (ball) wheel | |
| Distance sensor | HC-SR04 Ultrasonic | |
| IR receiver | Keyestudio KS0026 (38kHz) | 2nd unit — the first one burned out during diagnostics |
| Remote control | IR remote from the ELEGOO Super Starter kit | NEC protocol |
| Power supply | 6× AA battery pack (9V, snap-on connector) | Replaced the original 9V block battery — 5× more runtime |
| Chassis | Rigid cube-shaped structure | Assembled with hot glue and tape |

📄 Full bill of materials in [`docs/materials.md`](docs/materials.md)

---

## 🔌 Wiring Summary

| Arduino | Component | Pin |
|---|---|---|
| D2 | IR receiver | Signal |
| D4, D5 | L298N | IN1, IN2 (Right Motor) |
| D6, D7 | L298N | IN3, IN4 (Left Motor) |
| D9 *(PWM)* | L298N | ENB (Left Motor speed) |
| D10 *(PWM)* | L298N | ENA (Right Motor speed) |
| D12 | HC-SR04 | TRIG |
| D8 | HC-SR04 | ECHO |
| 5V / GND | L298N, HC-SR04, IR | Logic power / ground |
| VIN | L298N | VCC (motor power) |

📄 Full wire-by-wire wiring diagram in [`docs/wiring.md`](docs/wiring.md)

⚠️ Important notes:
- **ECHO is on D8, not D13** — D13 is the Arduino's built-in LED pin and conflicts with the IRremote library.
- **ENA is on D10, not D3** — D3 shares Timer2 with the IRremote library.
- The L298N's 3 jumpers (5V Enable, ENA, ENB) were removed to allow external power and PWM control.

---

## 💻 Software

| File | Description |
|---|---|
| [`src/robo_cubo.ino`](src/robo_cubo.ino) | **Final code** — the complete, current version of the project |
| `src/development_phases/` | Intermediate versions from each development phase, for reference |

### Required Libraries

- [IRremote](https://github.com/Arduino-IRremote/Arduino-IRremote) (v3.x) — install via Arduino IDE: `Tools → Manage Libraries → "IRremote"`

### Remote Button Mapping

| Button | Action |
|---|---|
| 2 | Forward |
| 8 | Backward |
| 4 | Turn left |
| 6 | Turn right |
| 5 | Stop |
| + | Increase speed |
| − | Decrease speed |
| 0 | Toggle Manual ↔ Autopilot |
| CH− | Emergency stop |

---

## 🛠️ Development Process

The project was built in 5 incremental phases, each tested in isolation before moving on:

1. **Motors** — basic direction and speed control via L298N
2. **Ultrasonic Sensor** — distance reading and automatic stop
3. **IR Remote** — manual remote control, with button mapping via a decoder
4. **Autopilot** — non-blocking state machine for autonomous navigation
5. **Final Integration** — combining everything into a single sketch with mode switching

📄 Full history of issues encountered and resolved in [`docs/issues-resolved.md`](docs/issues-resolved.md) — includes 2 damaged components, pin conflicts, reversed motors, and electromagnetic interference.

---

## 📂 Repository Structure

```
RoboCubo/
├── README.md                          ← this file
├── src/
│   ├── robo_cubo.ino                  ← final code
│   └── development_phases/            ← versions from each phase
│       ├── phase1_motors.ino
│       ├── phase2_sensor.ino
│       ├── phase3a_decoder.ino
│       ├── phase3_ir_control.ino
│       └── phase4_autopilot.ino
└── docs/
    ├── materials.md                   ← bill of materials and costs
    ├── wiring.md                      ← wire-by-wire wiring diagram
    └── issues-resolved.md             ← history of bugs and fixes
```

---

## 🚀 How to Use

1. Assemble the hardware following [`docs/wiring.md`](docs/wiring.md)
2. Install the **IRremote v3.x** library in the Arduino IDE
3. Upload `src/robo_cubo.ino` to the Arduino Uno
4. Connect the battery (6× AA) to the barrel jack
5. The robot starts in **Manual mode** — use the remote to test movement
6. Press **0** to enable autopilot

---

## 💡 Possible Future Improvements

- Add wheel encoders for precise turning-angle calibration
- Bluetooth/App control instead of IR
- Additional rear sensor for obstacle detection while reversing
- Extra filter capacitor on the L298N to reduce EMI

---

## 📜 License

Personal learning project — free to use, modify, and share.
