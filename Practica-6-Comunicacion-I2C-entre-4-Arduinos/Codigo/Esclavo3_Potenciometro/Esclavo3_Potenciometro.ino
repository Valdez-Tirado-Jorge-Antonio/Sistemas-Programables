// Esclavo 3 (0x0A) - lee un potenciometro y envia su valor (0-1023) al
// maestro cuando este lo pide, en 2 bytes.
//
// La lectura se hace en loop(), NO dentro de onRequest: en la R4 WiFi esa
// rutina corre dentro de la interrupcion del I2C y un analogRead() ahi
// retrasa la respuesta, asi que el maestro recibia bytes basura (valores
// como 20548, imposibles para un potenciometro). Ademas los 2 bytes se
// mandan en UN solo Wire.write para que viajen juntos.
#include <Wire.h>

volatile uint16_t ultimoValor = 0;

void setup() {
  Wire.begin(0x0A);
  Wire.onRequest(enviar);
}

void loop() {
  uint16_t v = analogRead(A0);  // 0 a 1023
  noInterrupts();               // que la ISR no lea el valor a medio copiar
  ultimoValor = v;
  interrupts();
  delay(5);
}

void enviar() {
  uint16_t v = ultimoValor;
  byte datos[2] = { highByte(v), lowByte(v) };
  Wire.write(datos, 2);
}
