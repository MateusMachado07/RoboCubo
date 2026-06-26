// ============================================================
// ROBÔ CUBO — Fase 3a: Decoder IR
// ============================================================
// Objetivo: identificar os códigos de cada botão do teu
// comando ELEGOO antes de programar o controlo completo.
//
// Como usar:
// 1. Carrega este código para o Arduino
// 2. Abre o Serial Monitor a 9600 baud
// 3. Aponta o comando ao receptor IR e pressiona cada botão
// 4. Anota o código que aparece para cada botão
// ============================================================

#include <IRremote.hpp>

#define IR_PIN  7   // D7: confirmado a funcionar com este módulo IR

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

  Serial.println("================================");
  Serial.println(" Robô Cubo — Fase 3a: Decoder");
  Serial.println("================================");
  Serial.println("Aponta o comando e pressiona");
  Serial.println("cada botao...");
  Serial.println();
}

void loop() {
  if (IrReceiver.decode()) {
    uint32_t codigo = IrReceiver.decodedIRData.decodedRawData;

    // Ignora código de repetição (botão mantido pressionado)
    if (codigo != 0xFFFFFFFF && codigo != 0) {
      Serial.print("Botao pressionado → codigo: 0x");
      Serial.println(codigo, HEX);
    }

    IrReceiver.resume(); // Prepara para o próximo sinal
  }
}
