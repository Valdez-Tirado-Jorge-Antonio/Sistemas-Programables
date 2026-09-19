Diagrama del circuito.

  diagrama-tinkercad.jpg  - Diagrama de conexiones hecho en Tinkercad: Arduino UNO R4 WiFi,
                            pulsador en la protoboard entre el pin 2 y GND   [LISTO]

Conexion:
  Pulsador   pin 2 a GND (INPUT_PULLUP, interrupcion FALLING), sin resistencia
  Matriz     integrada en el UNO R4 WiFi, no lleva cables

El diagrama de flujo (pulsador -> ISR -> buffer -> loop) y el esquema del buffer
circular estan en el Readme.md principal (mermaid, se ve en GitHub).
