/*
  Practica - Semaforo vehicular y peatonal con maquina de estados finitos (FSM)

  El semaforo vehicular cicla VERDE -> AMARILLO -> ROJO -> VERDE por si solo. Un
  pulsador arma una solicitud de cruce peatonal; la solicitud solo se acepta durante
  VERDE o AMARILLO y se atiende cuando el vehicular llega a ROJO: ahi el peatonal pasa
  a VERDE un tiempo fijo, vuelve a ROJO y el ciclo vehicular reinicia. Si nadie
  presiona, el ciclo sigue sin detenerse.

  Restricciones: sin delay() (todo con millis()) y estados en un enum class.

  Placa: Arduino UNO R4 WiFi
*/

// ---- Pines ----------------------------------------------------------------
const int PIN_VEH_VERDE    = 11;
const int PIN_VEH_AMARILLO = 12;
const int PIN_VEH_ROJO     = 13;
const int PIN_PEA_ROJO     = 8;
const int PIN_PEA_VERDE    = 7;
const int PIN_BOTON        = 2;   // pulsador a GND, con INPUT_PULLUP (presionado = LOW)

// ---- Duraciones (ms) ------------------------------------------------------
const unsigned long T_VERDE    = 6000;
const unsigned long T_AMARILLO = 2000;   // claramente mas corto que los otros dos
const unsigned long T_ROJO     = 5000;   // rojo vehicular cuando NO hay peaton
const unsigned long T_PEATON   = 5000;   // verde peatonal (el vehicular sigue en rojo)
const unsigned long T_REBOTE   = 40;     // antirrebote del pulsador

// ---- Estados de la FSM ----------------------------------------------------
enum class Estado {
  VEH_VERDE,     // vehicular verde,    peatonal rojo
  VEH_AMARILLO,  // vehicular amarillo, peatonal rojo
  VEH_ROJO,      // vehicular rojo,     peatonal rojo (sin solicitud)
  PEA_VERDE      // vehicular rojo,     peatonal verde (atendiendo la solicitud)
};

Estado estado = Estado::VEH_VERDE;
unsigned long inicioEstado = 0;   // millis() en que se entro al estado actual
bool solicitudPeatonal = false;   // "armada": se atiende al llegar a rojo

// Antirrebote no bloqueante del pulsador
int lecturaEstable = HIGH;
int lecturaAnterior = HIGH;
unsigned long ultimoCambioLectura = 0;

const char* nombre(Estado e) {
  switch (e) {
    case Estado::VEH_VERDE:    return "VEH_VERDE";
    case Estado::VEH_AMARILLO: return "VEH_AMARILLO";
    case Estado::VEH_ROJO:     return "VEH_ROJO";
    case Estado::PEA_VERDE:    return "PEA_VERDE";
  }
  return "?";
}

// Enciende los 5 LEDs segun el estado. Es la unica funcion que toca las salidas,
// asi que las luces siempre son coherentes con el estado.
void aplicarSalidas(Estado e) {
  digitalWrite(PIN_VEH_VERDE,    e == Estado::VEH_VERDE);
  digitalWrite(PIN_VEH_AMARILLO, e == Estado::VEH_AMARILLO);
  digitalWrite(PIN_VEH_ROJO,     e == Estado::VEH_ROJO || e == Estado::PEA_VERDE);
  digitalWrite(PIN_PEA_VERDE,    e == Estado::PEA_VERDE);
  digitalWrite(PIN_PEA_ROJO,     e != Estado::PEA_VERDE);
}

void cambiarEstado(Estado nuevo, unsigned long ahora) {
  estado = nuevo;
  inicioEstado = ahora;
  aplicarSalidas(estado);
  Serial.print(ahora);
  Serial.print(" ms  -> ");
  Serial.print(nombre(estado));
  if (solicitudPeatonal) Serial.print("  (solicitud peatonal armada)");
  Serial.println();
}

// Devuelve true solo en el instante en que el boton pasa de suelto a presionado,
// ya filtrado el rebote. No bloquea.
bool botonPresionado(unsigned long ahora) {
  int lectura = digitalRead(PIN_BOTON);
  if (lectura != lecturaAnterior) {
    ultimoCambioLectura = ahora;
    lecturaAnterior = lectura;
  }
  if (ahora - ultimoCambioLectura >= T_REBOTE && lectura != lecturaEstable) {
    lecturaEstable = lectura;
    return lecturaEstable == LOW;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_VEH_VERDE, OUTPUT);
  pinMode(PIN_VEH_AMARILLO, OUTPUT);
  pinMode(PIN_VEH_ROJO, OUTPUT);
  pinMode(PIN_PEA_ROJO, OUTPUT);
  pinMode(PIN_PEA_VERDE, OUTPUT);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  Serial.println("Semaforo FSM (sin delay)");
  cambiarEstado(Estado::VEH_VERDE, millis());
}

void loop() {
  unsigned long ahora = millis();
  unsigned long enEstado = ahora - inicioEstado;

  // 1) Entrada: el boton solo arma la solicitud en VERDE o AMARILLO. En ROJO o con el
  //    peatonal ya en verde se ignora (no interrumpe nada de golpe).
  if (botonPresionado(ahora)) {
    if (estado == Estado::VEH_VERDE || estado == Estado::VEH_AMARILLO) {
      if (!solicitudPeatonal) {
        solicitudPeatonal = true;
        Serial.print(ahora);
        Serial.println(" ms  boton: solicitud peatonal ARMADA, se atiende en rojo");
      }
    } else {
      Serial.print(ahora);
      Serial.println(" ms  boton ignorado (fuera de verde/amarillo)");
    }
  }

  // 2) Transiciones por tiempo
  switch (estado) {
    case Estado::VEH_VERDE:
      if (enEstado >= T_VERDE) cambiarEstado(Estado::VEH_AMARILLO, ahora);
      break;

    case Estado::VEH_AMARILLO:
      if (enEstado >= T_AMARILLO) {
        // Al llegar a rojo se decide: con solicitud, cruce peatonal; sin ella, rojo normal
        if (solicitudPeatonal) {
          solicitudPeatonal = false;
          cambiarEstado(Estado::PEA_VERDE, ahora);
        } else {
          cambiarEstado(Estado::VEH_ROJO, ahora);
        }
      }
      break;

    case Estado::VEH_ROJO:
      if (enEstado >= T_ROJO) cambiarEstado(Estado::VEH_VERDE, ahora);
      break;

    case Estado::PEA_VERDE:
      if (enEstado >= T_PEATON) cambiarEstado(Estado::VEH_VERDE, ahora);
      break;
  }
}
