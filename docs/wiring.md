# 🔌 Wiring Diagram

## ⚠️ Before Connecting — L298N Preparation

The L298N module ships with **3 jumpers** that must be removed:

| Jumper | Location | Why remove it |
|---|---|---|
| 5V Enable | Power terminal | Allows connecting the Arduino's 5V for logic power |
| ENA | Next to the ENA pin | Allows PWM speed control of the Right Motor |
| ENB | Next to the ENB pin | Allows PWM speed control of the Left Motor |

---

## 🔋 Power

| From | To | Connector |
|---|---|---|
| 6× AA pack (snap-on clip) | Arduino | Barrel jack (direct, no extra wires) |
| Arduino VIN | L298N VCC | Blue screw terminal |
| Arduino GND | L298N GND | Blue screw terminal |
| Arduino 5V | L298N +5V | Header pin |

> 💡 The L298N's red LED should light up as soon as the battery is connected — that's the visual confirmation that power is correct before testing the motors.

---

## ⚡ L298N → Motors (Blue Screw Terminals)

| L298N | Connection |
|---|---|
| OUT1 | Right Motor (+) |
| OUT2 | Right Motor (−) |
| OUT3 | Left Motor (+) |
| OUT4 | Left Motor (−) |

> ⚠️ **Mapping confirmed by physical testing** — channel A (ENA/IN1/IN2/OUT1/OUT2) ended up wired to the motor that is physically on the **right** side of the robot, and channel B to the **left** motor. If your physical build differs, test each motor individually before assuming this mapping (see "Calibration" below).

---

## 🎮 Arduino → L298N (Control Pins)

| Arduino | L298N | Function |
|---|---|---|
| **D10** *(PWM)* | ENA | Right Motor speed |
| D4 | IN1 | Right Motor direction |
| D5 | IN2 | Right Motor direction |
| D6 | IN3 | Left Motor direction |
| D7 | IN4 | Left Motor direction |
| **D9** *(PWM)* | ENB | Left Motor speed |

> ⚠️ **ENA is on D10, not D3.** D3 shares Timer2 with the IRremote library — using D3 for motor PWM causes the IR receiver to stop working whenever the motors are running.

---

## 📡 Arduino → HC-SR04 (Ultrasonic Sensor)

| Arduino | HC-SR04 |
|---|---|
| 5V | VCC |
| GND | GND |
| D12 | TRIG |
| **D8** | ECHO |

> ⚠️ **ECHO is on D8, not D13.** D13 is also the Arduino's built-in LED pin (`LED_BUILTIN`). The IRremote library uses this pin internally even with LED feedback disabled (`DISABLE_LED_FEEDBACK`), which directly conflicts with reading the sensor — causing readings to always return the timeout value (999cm), as if there were never an obstacle.

---

## 📻 Arduino → IR Receiver (Keyestudio KS0026)

| Module Pin | Meaning | Connect to |
|---|---|---|
| S | Signal | Arduino D2 |
| V | VCC | Arduino 5V |
| G | GND | Arduino GND |

> 💡 The IR receiver's LED is a **signal indicator**, not a power indicator — it lights up when the Signal pin is LOW (off at rest; blinks rapidly when receiving a command).

---

## 📊 Summary Table — All Arduino Pins

| Pin | Component | Function |
|---|---|---|
| D2 | IR receiver | Signal |
| D4 | L298N | IN1 — Right Motor direction |
| D5 | L298N | IN2 — Right Motor direction |
| D6 | L298N | IN3 — Left Motor direction |
| D7 | L298N | IN4 — Left Motor direction |
| D8 | HC-SR04 | ECHO |
| D9 *(PWM)* | L298N | ENB — Left Motor speed |
| D10 *(PWM)* | L298N | ENA — Right Motor speed |
| D12 | HC-SR04 | TRIG |
| 5V | L298N, HC-SR04, IR | Logic power / sensors |
| GND | L298N, HC-SR04, IR | Common ground |
| VIN | L298N | VCC (motor power) |
| Barrel jack | 6× AA pack | Main power (9V) |

**Free pins for expansion:** D0, D1, D3, D11, D13, A0–A5
*(D3 and D11 share Timer2 with IRremote — avoid for PWM if the library is in use)*

---

## 🧪 Calibration — If Motors Spin in the Wrong Direction

DC motors tolerate reversed wiring without any damage. To calibrate:

1. Disconnect power
2. Upload a simple sketch that drives one motor at a time (e.g., `IN1=HIGH, IN2=LOW, analogWrite(ENA, 150)`)
3. Observe the actual direction and adjust the logic in the `forward()`, `backward()`, `turnLeft()`, `turnRight()` functions in the code — or swap the two wires for that motor at the blue terminal on the L298N (OUT1↔OUT2 or OUT3↔OUT4)

> 💡 It's faster to fix this in code than to physically rewire.

---

## ✅ Safety Checklist Before Powering On

- [ ] L298N's 3 jumpers removed (5V Enable, ENA, ENB)
- [ ] All blue terminal screws tightened
- [ ] ECHO on D8 (not D13)
- [ ] ENA on D10 (not D3)
- [ ] No loose wires touching each other
- [ ] All signal connections made **before** connecting VIN/GND/5V

> 🔒 **Golden rule:** always disconnect power before touching any wire. Most components damaged during prototyping result from connections made while current is flowing.
