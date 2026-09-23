// Maestro I2C - pide el potenciometro al esclavo 3, mueve el servo del
// esclavo 2 y prende/apaga el LED del esclavo 1 por Monitor Serie.
// Ajustado para Arduino UNO R4 WiFi (ver Codigo/Readme.txt).
#include <Wire.h>

const uint8_t DIR_LED = 0x08;    // Esclavo 1
const uint8_t DIR_SERVO = 0x09;  // Esclavo 2
const uint8_t DIR_POT = 0x0A;    // Esclavo 3

unsigned long ultimaLectura = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000) { }  // espera el USB nativo del R4 (max. 3 s)
  Wire.begin();  // sin direccion = maestro
  Serial.println("Maestro listo. Escribe 1 (LED encendido) o 0 (LED apagado)");
}

void loop() {
  // --- Orden por el Monitor serie: enciende/apaga el LED del esclavo 1 ---
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '1' || c == '0') {
      byte orden = (c == '1') ? 1 : 0;
      Wire.beginTransmission(DIR_LED);
      Wire.write(orden);
      if (Wire.endTransmission() != 0) {
        Serial.println("Esclavo 1 (LED) no responde");
      }
    }
  }

  // --- Cada 500 ms: pide el potenciometro al esclavo 3 y mueve el servo ---
  if (millis() - ultimaLectura >= 500) {
    ultimaLectura = millis();

    if (Wire.requestFrom(DIR_POT, 2) == 2) {
      byte alto = Wire.read();
      byte bajo = Wire.read();
      int valor = (alto << 8) | bajo;  // 0 a 1023

      // Un potenciometro nunca pasa de 1023: si llega algo mayor es ruido en
      // el bus (faltan pull-ups, GND no comun, el servo metiendo ruido...).
      // Se reporta y NO se manda al servo, para que no brinque.
      if (valor > 1023) {
        Serial.print("Dato invalido del esclavo 3: ");
        Serial.println(valor);
        return;
      }
      byte angulo = map(valor, 0, 1023, 0, 180);

      Wire.beginTransmission(DIR_SERVO);
      Wire.write(angulo);
      if (Wire.endTransmission() != 0) {
        Serial.println("Esclavo 2 (servo) no responde");
      }

      Serial.print("Potenciometro: ");
      Serial.print(valor);
      Serial.print(" -> servo a ");
      Serial.print(angulo);
      Serial.println(" grados");
    } else {
      Serial.println("Esclavo 3 (potenciometro) no responde");
    }
  }
}
