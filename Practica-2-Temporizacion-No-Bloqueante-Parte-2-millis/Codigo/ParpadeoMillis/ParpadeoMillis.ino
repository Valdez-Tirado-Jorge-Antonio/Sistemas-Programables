/*
  Practica 2.1.1 - Temporizacion no bloqueante - Parte 2: millis()

  LED1 parpadea cada 500 ms, LED2 cada 1000 ms y LED3 cada 1500 ms, los tres al
  mismo tiempo y cada uno con su propio ritmo. No hay un solo delay(): cada tarea
  guarda el instante en que actuo por ultima vez y, en cada vuelta del loop(),
  pregunta si ya le toca otra vez.

  Reto opcional: una cuarta tarea imprime en el Monitor Serie cada 3000 ms. Se agrego
  sin tocar el codigo de los LEDs, solo sumando una entrada mas a la lista de tareas.

  Placa: Arduino UNO R4 WiFi
*/

// Una tarea periodica: que pin controla, cada cuanto y cuando fue la ultima vez
struct Parpadeo {
  int pin;
  unsigned long periodoMs;
  unsigned long ultimoCambio;
  bool encendido;
};

Parpadeo leds[] = {
  { 8,  500, 0, false },   // LED1
  { 9, 1000, 0, false },   // LED2
  { 10, 1500, 0, false },  // LED3
};
const int NUM_LEDS = sizeof(leds) / sizeof(leds[0]);

// Cuarta tarea (reto opcional): mensaje por serial cada 3 s
const unsigned long PERIODO_SERIAL_MS = 3000;
unsigned long ultimoMensaje = 0;
unsigned long contadorMensajes = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(leds[i].pin, OUTPUT);
  }
  Serial.println("Parpadeo con millis(): 500 / 1000 / 1500 ms");
}

void loop() {
  unsigned long ahora = millis();

  // Tareas 1 a 3: los LEDs. La resta (ahora - ultimoCambio) en unsigned long sigue
  // siendo correcta cuando millis() se desborda a los ~49.7 dias.
  for (int i = 0; i < NUM_LEDS; i++) {
    if (ahora - leds[i].ultimoCambio >= leds[i].periodoMs) {
      leds[i].ultimoCambio = ahora;
      leds[i].encendido = !leds[i].encendido;
      digitalWrite(leds[i].pin, leds[i].encendido ? HIGH : LOW);
    }
  }

  // Tarea 4: serial. Independiente de las anteriores.
  if (ahora - ultimoMensaje >= PERIODO_SERIAL_MS) {
    ultimoMensaje = ahora;
    contadorMensajes++;
    Serial.print("Mensaje #");
    Serial.print(contadorMensajes);
    Serial.print("  t = ");
    Serial.print(ahora);
    Serial.print(" ms  LEDs: ");
    for (int i = 0; i < NUM_LEDS; i++) {
      Serial.print(leds[i].encendido ? "1" : "0");
    }
    Serial.println();
  }
}
