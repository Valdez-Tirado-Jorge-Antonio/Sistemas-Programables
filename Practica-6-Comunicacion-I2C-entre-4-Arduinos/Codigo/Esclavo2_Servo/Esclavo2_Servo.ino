// Esclavo 2 (0x09) - recibe un angulo (0-180) del maestro y mueve un servomotor.
#include <Wire.h>
#include <Servo.h>

Servo miServo;
volatile byte angulo = 90;
volatile bool hayDato = false;

void setup() {
  miServo.attach(9);
  miServo.write(90);
  Wire.begin(0x09);
  Wire.onReceive(recibir);
}

void loop() {
  // El servo se mueve aqui, no dentro de la rutina de recepcion.
  if (hayDato) {
    hayDato = false;
    miServo.write(angulo);
  }
}

void recibir(int cuantos) {
  while (Wire.available()) {
    int a = Wire.read();  // se lee UNA sola vez
    if (a > 180) a = 180;
    if (a < 0) a = 0;
    angulo = a;
    hayDato = true;
  }
}
