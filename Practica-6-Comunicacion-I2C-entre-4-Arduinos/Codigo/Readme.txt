Codigo fuente de la practica: 4 sketches, uno por placa (maestro + 3 esclavos).
Cada uno vive en su propia carpeta porque el IDE de Arduino exige que el
archivo .ino se llame igual que la carpeta que lo contiene.

  Maestro/Maestro.ino
    Pide el potenciometro al esclavo 3 cada 500 ms (con millis(), sin
    bloquear), lo manda como angulo al esclavo 2, y atiende el Monitor Serie
    para prender/apagar el LED del esclavo 1 (escribir 1 o 0).   [LISTO]

  Esclavo1_LED/Esclavo1_LED.ino  (direccion 0x08)
    Prende o apaga un LED segun la orden que le manda el maestro por
    Wire.onReceive.   [LISTO]

  Esclavo2_Servo/Esclavo2_Servo.ino  (direccion 0x09)
    Recibe un angulo 0-180 y mueve un servomotor (pin 9). El movimiento se
    hace en loop(), no dentro de la rutina de recepcion.   [LISTO]

  Esclavo3_Potenciometro/Esclavo3_Potenciometro.ino  (direccion 0x0A)
    Lee el potenciometro en A0 y lo manda en 2 bytes cuando el maestro lo
    pide con Wire.onRequest.   [LISTO]

Los 4 sketches se compilaron y verificaron con arduino-cli para la placa
"arduino:renesas_uno:unor4wifi" (22-28% de programa, 27-28% de memoria
dinamica cada uno). Requiere la libreria "Servo" instalada aparte (el core
de la R4 no la trae, a diferencia de Wire).

Ajustes hechos por pasar de UNO R3 (consigna original) a UNO R4 WiFi:
  1. El maestro espera a que se abra el puerto serie USB nativo antes de
     imprimir (while (!Serial && millis() < 3000)), porque si no los
     primeros mensajes se pierden.
  2. La resistencia del LED sube de 220 a 470 ohm (los pines de la R4
     aguantan menos corriente).
  3. Las resistencias de pull-up de 4.7 kohm en SDA y SCL dejan de ser
     opcionales (fuera del simulador; Tinkercad no las exige).
  4. En el esclavo del LED, la constante de pin NO se puede llamar PIN_LED:
     ese nombre ya lo define el core de la R4 para el LED integrado y el
     programa no compilaba ("expected unqualified-id before numeric
     constant", verificado con arduino-cli). Se renombro a PIN_LED_OUT, en
     el pin 8.

PENDIENTE: confirmar contra el proyecto real de Tinkercad que el LED del
esclavo 1 esta cableado al mismo pin que usa el codigo (aqui, pin 8) y
ajustarlo si no coincide.
