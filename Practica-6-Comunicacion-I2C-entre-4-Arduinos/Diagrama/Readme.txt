Diagrama del circuito.

  diagrama-conexion-i2c.png  - Captura del bus armado en Tinkercad: 4x Arduino
    UNO, protoboard con LED + resistencia y potenciometro, micro servo, y las
    lineas SDA/SCL/GND compartidas entre las cuatro placas.   [LISTO]

Conexion:
  SDA (A4)   comun a las 4 placas
  SCL (A5)   comun a las 4 placas
  GND        comun a las 4 placas (los 5V NO se conectan entre placas: cada
             una se alimenta por su propio USB)
  LED        esclavo 1, con resistencia de 470 ohm (ver Codigo/Readme.txt)
  Servo      esclavo 2, senal en el pin 9
  Potenciometro  esclavo 3, senal en A0
