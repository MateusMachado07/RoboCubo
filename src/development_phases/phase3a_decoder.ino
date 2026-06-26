// ============================================================
// ROBÔ CUBO — Phase 3a: IR Decoder
// ============================================================
// Goal: identify the codes for each button on your ELEGOO
// remote before programming full control.
//
// How to use:
// 1. Upload this code to the Arduino
// 2. Open the Serial Monitor at 9600 baud
// 3. Point the remote at the IR receiver and press each button
// 4. Note the code shown for each button
// ============================================================

#include <IRremote.hpp>

#define IR_PIN  7   // D7: confirmed working with this IR module

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

  Serial.println("================================");
  Serial.println(" Robo Cubo — Phase 3a: Decoder");
  Serial.println("================================");
  Serial.println("Point the remote and press");
  Serial.println("each button...");
  Serial.println();
}

void loop() {
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;

    // Ignore repeat code (button held down)
    if (code != 0xFFFFFFFF && code != 0) {
      Serial.print("Button pressed -> code: 0x");
      Serial.println(code, HEX);
    }

    IrReceiver.resume(); // Ready for the next signal
  }
}
