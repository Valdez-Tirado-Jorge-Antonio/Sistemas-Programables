Codigo fuente de la practica.

  SemaforoFSM/SemaforoFSM.ino  - Semaforo vehicular + peatonal como maquina de estados
                                 (enum class Estado), todo con millis(), sin delay().

Los tiempos de cada fase son las constantes T_VERDE, T_AMARILLO, T_ROJO y T_PEATON
arriba del .ino. Los pines tambien estan ahi (PIN_...).

El boton usa INPUT_PULLUP: va del pin 2 a GND, sin resistencia.

El .ino va dentro de una carpeta con su mismo nombre porque asi lo exige el IDE de Arduino.
Placa requerida en el Gestor de Tarjetas: "Arduino UNO R4 Boards".
