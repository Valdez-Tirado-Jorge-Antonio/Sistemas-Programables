// Esclavo 1 (0x08) - recibe una orden del maestro (1/0) y prende o apaga un LED.
#include <Wire.h>

// En la R4 WiFi el nombre PIN_LED ya esta definido por el core (LED integrado),
// asi que la constante propia se llama distinto para no chocar con esa macro.
const uint8_t PIN_LED_OUT = 8;

void setup() {
  pinMode(PIN_LED_OUT, OUTPUT);
  Wire.begin(0x08);  // direccion de este esclavo
  Wire.onReceive(recibir);
}

void loop() {
}

void recibir(int cuantos) {
  while (Wire.available()) {
    byte dato = Wire.read();
    digitalWrite(PIN_LED_OUT, dato ? HIGH : LOW);
  }
}
