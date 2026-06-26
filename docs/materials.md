# 📦 Materials and Costs

## Reused Components (ELEGOO Super Starter kit)

| Component | Quantity |
|---|---|
| Arduino Uno R3 | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| IR remote control (NEC protocol) | 1 |
| MM and MF jumper wires | as needed |
| Breadboard | 1 |

## Purchased Components

| Component | Spec | Approx. Cost |
|---|---|---|
| Motor + driver kit | 2× TT DC Motor 1:48 (3-6V) + 2× wheels + L298N + jumper cables | €13 |
| IR receiver | Keyestudio KS0026 (38kHz) | €1–2 |
| IR receiver (replacement) | Keyestudio KS0026 — 1st unit damaged during diagnostics | included above |
| Motor Driver (replacement) | Keyestudio KS0063 (L298N) — 1st L298N damaged | €4–5 |
| Battery holder | 6× AA with snap-on (PP3) connector | €3–5 |
| AA batteries | 6× — replaced the original 9V block battery | €3–5 |

**Total spent on the project: ~€26**

## Other Materials (already on hand)

- Cube-shaped chassis (robot body)
- Caster (ball) wheel for rear balance
- Hot glue gun
- Tape
- Ruler and pencil for measurements

## Substitution Decisions

| Originally Planned Component | Replaced With | Reason |
|---|---|---|
| 2× 28BYJ-48 stepper motors + 2× ULN2003 | 2× TT DC Motor (1:48) + L298N | Much higher speed, PWM speed control, cheaper |
| 9V block battery | 6× AA battery pack | Block battery has insufficient current for the motors (~500mAh vs ~2500mAh of the AA pack); runtime went from ~20-30 min to ~2-3 hours |
