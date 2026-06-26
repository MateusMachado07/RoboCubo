# 🔧 Issues Encountered and Resolved

A history of the main obstacles faced during development — documented for future reference and for anyone replicating the project.

---

## 1. Motors Too Slow (Initial Plan)

**Problem:** The initial plan used 28BYJ-48 stepper motors with a ULN2003 driver — functional, but far too slow for a moving robot.

**Solution:** Replaced with TT DC motors (1:48) and an L298N driver — adequate speed with PWM speed control.

---

## 2. IR Receiver Not Detecting Signal

**Symptom:** No code appeared in the Serial Monitor when pressing the remote; the module's LED never lit up.

**Diagnosis:**
- Confirmed the remote was emitting an IR signal (visible via phone camera)
- Confirmed the module had power (LED lit when forcing the Signal pin to GND)
- Tested on multiple digital pins without success

**Cause:** The IR receiver was damaged, most likely from a momentary VCC/GND reversal during attempts to identify the correct pinout.

**Solution:** Replaced with a new module (Keyestudio KS0026), with a clearly labeled pinout (S/V/G) to avoid repeating the mistake.

**Lesson:** Chip-based components (sensors, receivers) don't tolerate reversed polarity — even momentarily. Always check the datasheet/pinout before powering up any new module.

---

## 3. L298N Damaged

**Symptom:** L298N's LED off, motors with no power (just a faint buzzing noise), chip warm but with no functional output.

**Cause:** Most likely an incorrect connection made momentarily while power was on, during one of the many IR receiver diagnostic attempts.

**Solution:** Replaced with a new L298N module (Keyestudio KS0063).

**Lesson:** Always disconnect power before touching any wire — even for changes that seem simple or quick.

---

## 4. Motors Wired with Swapped/Reversed Direction

**Symptom:** Pressing forward (button 2) made the robot turn right; pressing backward (button 8) made it turn left. The turn-left/turn-right buttons produced forward/backward movement instead.

**Diagnosis:** Tested each motor individually by forcing `IN1=HIGH` and then `IN3=HIGH`:
- Channel A (ENA/IN1/IN2) was actually wired to the **Right** motor, not the Left as initially assumed
- Direction on that channel was also reversed

**Solution:** Rewrote the 4 movement functions (`forward`, `backward`, `turnLeft`, `turnRight`) with corrected HIGH/LOW logic based on the actual physical test results, instead of assuming the theoretical mapping.

**Lesson:** When motor behavior doesn't match expectations, testing each motor in isolation (with direct `digitalWrite`, no other variables) is faster and more reliable than guessing the right combination.

---

## 5. Turning Too Fast for Precise Control

**Problem:** Using the same speed for forward/backward and turning made turns too abrupt for fine directional control.

**Solution:** Introduced a fixed, slower turning speed (`TURN_SPEED`), independent of the adjustable forward/backward speed controlled by the +/- buttons.

---

## 6. Sensor Always Reporting "No Obstacle" (999cm)

**Symptom:** In autopilot mode combined with IR control, the ultrasonic sensor never detected obstacles, even with a hand right against it — always 999cm.

**Diagnosis (by elimination):**
1. Tested the sensor alone (no IR) → worked perfectly
2. Tested the IR alone (no sensor) → worked perfectly
3. Tried pausing the IR receiver during sensor reads (`IrReceiver.stop()/start()`) → completely broke IR reception
4. Tried disabling interrupts during the read (`noInterrupts()/interrupts()`) → still unresolved
5. Direct debugging of sensor values confirmed: **always exactly 999.00**, consistently (not intermittent)

**Real cause:** The **ECHO_PIN was on D13**, which is also the Arduino's **built-in LED pin** (`LED_BUILTIN`). The IRremote library configures this pin internally for visual feedback, even with the `DISABLE_LED_FEEDBACK` option enabled (in some library versions) — creating a direct conflict with reading the sensor on the same pin.

**Solution:** Moved ECHO_PIN from D13 to D8 (a free, conflict-free pin).

**Lesson:** A reading that is **always identical and never varies** (rather than erratic/intermittent) is usually a sign of a hardware/pin configuration conflict, not noise or timing interference. It's worth checking whether a pin is being shared by two different libraries/functions before assuming more complex causes.

---

## 7. 9V Block Battery Draining Too Fast

**Problem:** The 9V block battery (PP3), despite having the correct voltage, has very low capacity (~500mAh) and limited continuous current (~200mA) — insufficient for 2 DC motors under sustained use.

**Solution:** Replaced with a 6× AA battery pack (also 9V in series), with a snap-on connector compatible with the existing cable — capacity of ~2500mAh, increasing runtime from ~20-30 minutes to ~2-3 hours.

---

## Summary of Damaged Components

| Component | Likely Cause | Replaced With |
|---|---|---|
| IR receiver (1st unit) | Polarity reversal during diagnostics | Keyestudio KS0026 |
| L298N module (1st unit) | Incorrect connection made while powered | Keyestudio KS0063 |

**Total replacement cost:** ~€5–7 (included in the project's overall budget)
