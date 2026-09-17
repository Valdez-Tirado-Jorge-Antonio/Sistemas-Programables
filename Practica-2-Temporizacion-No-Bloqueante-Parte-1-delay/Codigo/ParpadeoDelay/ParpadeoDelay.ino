/*
  Practica 2.1.1 - Temporizacion no bloqueante - Parte 1: el antipatron con delay()

  Intencion: LED1 cada 500 ms, LED2 cada 1000 ms, LED3 cada 1500 ms.

  Este es el enfoque "intuitivo": encender, esperar, apagar, esperar. NO funciona
  como se pretende. Cada delay() detiene por completo el programa, asi que los tres
  LEDs no pueden llevar su propio ritmo: todos quedan encadenados en una sola
  secuencia de 6000 ms (ver la tabla de tiempos en el Readme.md).

  Placa: Arduino UNO R4 WiFi
*/

const int LED1 = 8;
const int LED2 = 9;
const int LED3 = 10;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
  // "LED1 cada 500 ms"
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  delay(500);

  // "LED2 cada 1000 ms"  <- mientras se ejecuta esto, LED1 se queda apagado 2000 ms
  digitalWrite(LED2, HIGH);
  delay(1000);
  digitalWrite(LED2, LOW);
  delay(1000);

  // "LED3 cada 1500 ms"  <- y aqui LED1 y LED2 se quedan congelados 3000 ms mas
  digitalWrite(LED3, HIGH);
  delay(1500);
  digitalWrite(LED3, LOW);
  delay(1500);
}
