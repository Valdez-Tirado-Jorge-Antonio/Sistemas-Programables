Codigo fuente de la practica.

  BufferCircularISR/BufferCircularISR.ino  - Pulsador en pin de interrupcion -> ISR anota
                                             millis() en un buffer circular; el loop() anima
                                             la matriz de LEDs y vacia el buffer por serial.

Constantes arriba del .ino: TAM_BUFFER (8), T_REBOTE (50 ms), T_ANIMACION (60 ms),
T_ESTADO (5 s). El pin del pulsador es PIN_SENSOR (2).

El boton usa INPUT_PULLUP: va del pin 2 a GND, sin resistencia. La interrupcion es
en flanco FALLING (al presionar).

Usa la libreria Arduino_LED_Matrix que ya viene con el nucleo del UNO R4; no hay que
instalar nada extra.

El .ino va dentro de una carpeta con su mismo nombre porque asi lo exige el IDE de Arduino.
Placa requerida en el Gestor de Tarjetas: "Arduino UNO R4 Boards".
