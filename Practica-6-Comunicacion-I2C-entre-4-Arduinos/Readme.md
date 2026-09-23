# Nombre del proyecto
Comunicación I2C entre 4 Arduinos

## Descripción
Un Arduino maestro se comunica por el bus I2C (líneas SDA/SCL, tierra común)
con tres Arduinos esclavos, cada uno con su propia dirección (0x08, 0x09,
0x0A): el esclavo 1 prende/apaga un LED por orden del maestro, el esclavo 2
mueve un servomotor al ángulo que el maestro le manda, y el esclavo 3 lee un
potenciómetro y se lo entrega al maestro cuando este lo pide. Armado en
físico con 4× Arduino UNO R4 WiFi.

## Objetivos
- Comprender el funcionamiento del bus I2C entre un maestro y varios esclavos.
- Asignar direcciones distintas a cada esclavo y comunicarse con la correcta.
- Programar el envío (`Wire.write`) y la recepción (`Wire.onReceive`/`onRequest`)
  de datos entre placas.
- Detectar cuándo un esclavo no responde en vez de que el programa se cuelgue.

## Herramientas y material utilizado
- 4× Arduino UNO R4 WiFi
- 1 protoboard, 1 LED + resistencia de 470 Ω, 1 servomotor MG996R, 1 potenciómetro
- Librerías `Wire` y `Servo` (incluidas en el entorno de Arduino, `Servo` se
  instala aparte para el core de la R4)
- Arduino IDE y Monitor Serie

## Diagrama
Las cuatro placas comparten SDA (A4), SCL (A5) y tierra (GND); el maestro es
la que se alimenta por USB. En físico (fuera del simulador) esas líneas
necesitan pull-up de 4.7 kΩ a 5V, porque la R4 no las trae integradas.

![Diagrama de conexión](Diagrama/diagrama-conexion-i2c.png)

## Código
Cuatro programas independientes: el maestro consulta al esclavo del
potenciómetro cada 500 ms sin bloquear (`millis()`), reenvía el ángulo al
esclavo del servo y atiende el Monitor Serie para prender/apagar el LED del
esclavo 1. Ajustado para la UNO R4 WiFi (ver nota de compilación en
[Codigo/Readme.txt](Codigo/Readme.txt)).

[Ver código](Codigo/)

## Reporte
El reporte contiene la metodología del bus I2C, los ajustes hechos para la
R4 WiFi, el diagrama de conexión y las conclusiones técnicas.

[Ver Reporte](Reporte/Reporte-Comunicacion-I2C.pdf)

## Resultados
**Primera corrida (ver video): falló.** El bus sí comunicaba (ningún
esclavo reportó "no responde"), pero el Monitor Serie mostraba valores
imposibles del potenciómetro, del orden de `Potenciometro: 20548 -> servo a
23 grados` (el máximo real es 1023), y el servo brincaba a ángulos sin
relación con la perilla. Causa probable: el esclavo 3 hacía `analogRead()` dentro de
`onRequest`, que en la R4 corre dentro de la interrupción del I2C, y mandaba
los 2 bytes por separado; el maestro recibía bytes basura.

**Corrección:** el esclavo 3 ahora lee el potenciómetro en `loop()` y
responde con un valor ya listo en un solo `Wire.write(datos, 2)`; el maestro
descarta cualquier valor mayor a 1023 ("Dato invalido") en vez de mandarlo
al servo.

**Corrida con el código corregido: funciona.** El Monitor Serie reporta
valores dentro de rango y el ángulo correcto para cada uno (307 → 54°,
0 → 0°, 818 → 143°, 1023 → 180°), sin datos inválidos.

![Monitor Serie con el código corregido](Terminal/monitor-serie-codigo-corregido.jpeg)

## Video
[Ver video](https://youtu.be/-maYry-BLSA) · [Ver carpeta Video](Video/)

## Conclusiones
El bus I2C comunica varios dispositivos con solo dos líneas (SDA y SCL) más
tierra común: agregar un esclavo no pide pines nuevos, solo una dirección
distinta, y quien controla la conversación es siempre el maestro.

Revisar el resultado de `endTransmission()` y de `requestFrom()` es lo que
distingue un fallo detectado (esclavo desconectado, reportado por su nombre)
de un programa que se queda esperando una respuesta que nunca llega — la
misma lógica de "no confiar en que siempre funciona" que ya se usó con
`millis()` en la Práctica 2.

Dos errores quedaron documentados como los más comunes al armar este bus: una
resistencia con las dos patas en la misma columna de la protoboard no protege
nada (los huecos de una columna están unidos por dentro) y puede quemar el
LED; y leer el mismo dato de I2C dos veces dentro de la misma expresión
devuelve basura en la segunda lectura, porque el byte ya se consumió en la
primera.
