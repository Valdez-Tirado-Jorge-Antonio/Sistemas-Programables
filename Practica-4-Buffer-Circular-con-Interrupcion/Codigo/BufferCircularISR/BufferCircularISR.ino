/*
  Practica - Buffer circular alimentado por interrupcion externa

  Simula un sensor de piezas en una banda transportadora: cada pulsacion del boton
  es una pieza que pasa. Las piezas llegan en cualquier momento y el Arduino esta
  ocupado dibujando una animacion en la matriz de LEDs, asi que:

    1. El boton va en un pin de interrupcion externa. Al presionarlo el Arduino salta
       a la ISR sin importar en que parte del loop() este.
    2. La ISR solo anota el instante (millis()) en un buffer circular y sale. Trae su
       propio filtro de rebote porque un clic mecanico dispara varios flancos.
    3. El loop() sigue animando la matriz y, cuando tiene tiempo, vacia el buffer y
       reporta por serial cuantas piezas van.

  Lleno vs vacio: los dos indices apuntan al mismo lugar en ambos casos, asi que se
  sacrifica un hueco: el buffer esta VACIO cuando cabeza == cola y esta LLENO cuando
  (cabeza + 1) % TAM == cola. Con eso la ISR solo escribe `cabeza` y el loop() solo
  escribe `cola`, y nadie necesita apagar las interrupciones para leer el buffer.

  Placa: Arduino UNO R4 WiFi (matriz de 12x8 LEDs integrada)
*/

#include "Arduino_LED_Matrix.h"

// ---- Pines y tiempos ------------------------------------------------------
const int PIN_SENSOR = 2;                  // pulsador a GND, INPUT_PULLUP, flanco FALLING
const unsigned long T_REBOTE      = 50;    // ms: flancos mas juntos que esto son rebote
const unsigned long T_ANIMACION   = 60;    // ms entre cuadros de la animacion
const unsigned long T_ESTADO      = 5000;  // ms entre reportes de estado del buffer

// ---- Buffer circular ------------------------------------------------------
// TAM pequeno a proposito: para poder llenarlo a mano y ver que pasa.
const uint8_t TAM_BUFFER = 8;
volatile unsigned long buffer[TAM_BUFFER];   // instante (ms) de cada pieza
volatile uint8_t cabeza = 0;                 // siguiente lugar donde ESCRIBE la ISR
volatile uint8_t cola   = 0;                 // siguiente lugar que LEE el loop()
volatile unsigned long perdidas = 0;         // piezas que no cupieron (buffer lleno)
volatile unsigned long ultimoFlanco = 0;     // para el antirrebote dentro de la ISR

inline bool bufferVacio() { return cabeza == cola; }
inline bool bufferLleno() { return (uint8_t)((cabeza + 1) % TAM_BUFFER) == cola; }
inline uint8_t pendientes() { return (uint8_t)((cabeza + TAM_BUFFER - cola) % TAM_BUFFER); }

// ---- ISR: lo minimo posible -----------------------------------------------
// Sin Serial, sin delay, sin calculos. Solo: filtrar rebote, anotar, avanzar cabeza.
void ISR_pieza() {
  unsigned long ahora = millis();
  if (ahora - ultimoFlanco < T_REBOTE) return;   // rebote del mismo clic: se ignora
  ultimoFlanco = ahora;

  if (bufferLleno()) {        // no se pisa lo que el loop() aun no leyo
    perdidas++;
    return;
  }
  buffer[cabeza] = ahora;
  cabeza = (cabeza + 1) % TAM_BUFFER;
}

// ---- Animacion: una "pieza" recorriendo el borde de la matriz ------------
ArduinoLEDMatrix matriz;
uint8_t cuadro[8][12];
const uint8_t PERIMETRO = 2 * (12 + 8) - 4;   // 36 posiciones alrededor del borde
uint8_t posicion = 0;
unsigned long ultimoCuadro = 0;

// Convierte una posicion 0..35 del perimetro a (fila, columna)
void coordenada(uint8_t pos, uint8_t &fila, uint8_t &col) {
  if (pos < 12)      { fila = 0;         col = pos; }             // arriba, ->
  else if (pos < 19) { fila = pos - 11;  col = 11; }              // derecha, abajo
  else if (pos < 30) { fila = 7;         col = 11 - (pos - 18); } // abajo, <-
  else               { fila = 7 - (pos - 29); col = 0; }          // izquierda, arriba
}

void dibujarCuadro() {
  memset(cuadro, 0, sizeof(cuadro));
  uint8_t f, c;
  for (uint8_t i = 0; i < 3; i++) {                   // cabeza + estela de 2
    coordenada((posicion + PERIMETRO - i) % PERIMETRO, f, c);
    cuadro[f][c] = 1;
  }
  matriz.renderBitmap(cuadro, 8, 12);
  posicion = (posicion + 1) % PERIMETRO;
}

// ---- Programa principal ---------------------------------------------------
unsigned long piezas = 0;         // total procesadas por el loop()
unsigned long ultimaPieza = 0;    // instante de la anterior, para el intervalo
unsigned long ultimoEstado = 0;

void setup() {
  Serial.begin(9600);
  matriz.begin();
  pinMode(PIN_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR), ISR_pieza, FALLING);
  Serial.println("Buffer circular por interrupcion");
  Serial.print("TAM_BUFFER = "); Serial.print(TAM_BUFFER);
  Serial.print("  (capacidad util "); Serial.print(TAM_BUFFER - 1); Serial.println(")");
}

void loop() {
  unsigned long ahora = millis();

  // 1) La animacion nunca se detiene
  if (ahora - ultimoCuadro >= T_ANIMACION) {
    ultimoCuadro = ahora;
    dibujarCuadro();
  }

  // 2) Vaciar el buffer: procesar TODO lo que la ISR haya anotado desde la ultima vuelta
  while (!bufferVacio()) {
    unsigned long t = buffer[cola];
    cola = (cola + 1) % TAM_BUFFER;   // el loop() es el unico que mueve la cola
    piezas++;
    Serial.print("Pieza #"); Serial.print(piezas);
    Serial.print("  t = "); Serial.print(t); Serial.print(" ms");
    if (piezas > 1) {
      Serial.print("  (+"); Serial.print(t - ultimaPieza); Serial.print(" ms)");
    }
    Serial.println();
    ultimaPieza = t;
  }

  // 3) Reporte periodico del estado del buffer
  if (ahora - ultimoEstado >= T_ESTADO) {
    ultimoEstado = ahora;
    Serial.print("[estado] piezas: "); Serial.print(piezas);
    Serial.print("  pendientes: ");    Serial.print(pendientes());
    Serial.print("/");                  Serial.print(TAM_BUFFER - 1);
    Serial.print("  perdidas: ");      Serial.println(perdidas);
  }
}
