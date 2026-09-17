# Nombre del proyecto
Temporización no bloqueante — Parte 1: el antipatrón con `delay()`

## Descripción
Primera parte de la práctica 2.1.1. Se intenta que tres LEDs parpadeen a ritmos
distintos —**LED1 cada 500 ms, LED2 cada 1000 ms y LED3 cada 1500 ms**— con el enfoque
"intuitivo": encender, `delay()`, apagar, `delay()`, y así con cada LED.

El programa compila y los LEDs parpadean, pero **no a los ritmos pedidos**. `delay()`
detiene por completo la ejecución, así que mientras un LED "espera" los otros dos quedan
congelados en el estado en que estaban. El resultado es una sola secuencia de 6000 ms en
la que cada LED enciende una sola vez, en vez de tres tareas independientes.

Esta parte existe para ver el problema con los propios ojos; la
[Parte 2](../Practica-2-Temporizacion-No-Bloqueante-Parte-2-millis) lo resuelve con
`millis()` usando la misma conexión.

## Objetivos de aprendizaje
- Comprobar que `delay()` bloquea la ejecución del programa: durante la espera no se
  atiende nada más (ni otros LEDs, ni botones, ni el puerto serie).
- Entender por qué con retardos bloqueantes es imposible que varias tareas con
  periodos distintos convivan en un solo `loop()`.
- Reconocer el antipatrón para no repetirlo en sistemas que deban responder a
  varias cosas a la vez.

## Material utilizado

| Cantidad | Material |
|---|---|
| 1 | Arduino UNO R4 WiFi |
| 3 | LEDs (cualquier color) |
| 3 | Resistencias de 220 Ω |
| 1 | Protoboard |
| — | Cables Dupont macho-macho |

### Conexiones

| LED | Pin Arduino | Resistencia |
|---|---|---|
| LED 1 | 8 | 220 Ω a GND |
| LED 2 | 9 | 220 Ω a GND |
| LED 3 | 10 | 220 Ω a GND |

Cada LED va del pin a la pata larga (ánodo); la pata corta (cátodo) pasa por la
resistencia de 220 Ω a GND.

## Diagrama del circuito

![Diagrama de conexiones](Diagrama/diagrama-3-leds.png)

Es el mismo circuito de la Parte 2: solo cambia el programa.

```
Pin 8  ──┤>├──[220 Ω]──┐
Pin 9  ──┤>├──[220 Ω]──┤
Pin 10 ──┤>├──[220 Ω]──┴── GND
```

## Código
[ParpadeoDelay.ino](Codigo/ParpadeoDelay/ParpadeoDelay.ino)

### Por qué no funciona

El `loop()` completo tarda 500+500+1000+1000+1500+1500 = **6000 ms** en dar una
vuelta, y en esa vuelta cada LED enciende una sola vez:

| Tiempo (ms) | LED1 | LED2 | LED3 | Qué está haciendo el programa |
|---|---|---|---|---|
| 0 – 500 | ON | off | off | `delay(500)` |
| 500 – 1000 | off | off | off | `delay(500)` |
| 1000 – 2000 | off | ON | off | `delay(1000)` |
| 2000 – 3000 | off | off | off | `delay(1000)` |
| 3000 – 4500 | off | off | ON | `delay(1500)` |
| 4500 – 6000 | off | off | off | `delay(1500)` |

Lo que se pedía era que en esos mismos 6000 ms LED1 encendiera **6 veces**, LED2 **3** y
LED3 **2**. Con `delay()` no hay forma de hacerlo: el procesador está ocupado "sin hacer
nada" y no puede revisar si a otro LED ya le toca cambiar.

Se puede maquillar (usar un `delay(500)` como paso base y contadores para los otros
dos LEDs), pero sigue siendo bloqueante: en cuanto se agregue una tarea que no sea
múltiplo de 500 ms, o un botón que deba responder al instante, vuelve a fallar. La
solución de verdad es no esperar: la Parte 2.

## Video del funcionamiento

[![Ver en YouTube](https://img.youtube.com/vi/sUPkjtxan6g/hqdefault.jpg)](https://www.youtube.com/shorts/sUPkjtxan6g)

**YouTube:** https://www.youtube.com/shorts/sUPkjtxan6g

Copia local: [Video/parpadeo-delay.mp4](Video/parpadeo-delay.mp4) · [Readme](Video/Readme.txt)

## Evidencias de armado

Cuadros tomados del video del funcionamiento. Se ve que los LEDs encienden **uno
después del otro** (rojo, amarillo, verde), nunca dos a la vez: es la secuencia
encadenada que produce `delay()`.

| | | |
|---|---|---|
| ![LED rojo](Diagrama/foto-1-led-rojo.jpg) | ![LED amarillo](Diagrama/foto-2-led-amarillo.jpg) | ![LED verde](Diagrama/foto-3-led-verde.jpg) |

![Código en el IDE](Diagrama/foto-4-codigo-en-el-ide.jpg)

## Reporte
[Reporte de la práctica — Temporización con `delay()` (PDF)](Reporte/Reporte-Temporizacion-delay.pdf) · [qué contiene la carpeta](Reporte/Readme.txt)

Incluye:
- Datos generales, objetivo, tabla de conexiones y procedimiento
- Tabla de tiempos de los 6 s del `loop()` y comparación encendidos esperados contra reales
- Observaciones, conclusiones y evidencia del armado

## Conclusiones

`delay()` no significa "espera 500 ms para este LED": significa "detén todo el programa 500 ms". Mientras el procesador está dentro de un `delay()` no revisa ningún otro LED, ni un botón, ni el puerto serie; por eso los tres LEDs quedaron encadenados en una sola secuencia de 6 s y ninguno cumplió su periodo. En la protoboard se vio claro: encienden por turnos, nunca dos a la vez.

Con retardos bloqueantes es imposible que tareas con periodos distintos convivan en un solo `loop()`: el tiempo de cada una se suma al de las demás. Se puede maquillar con un paso base y contadores, pero en cuanto aparece una tarea que no es múltiplo de ese paso, o una entrada que deba atenderse al instante, vuelve a fallar.

En un sistema embebido real (sensores que muestrean, comunicación que llega en cualquier momento, botones del usuario) esta forma de programar es un antipatrón: el sistema se vuelve "sordo" durante cada espera. La solución no es acortar los `delay()` sino no esperar, que es lo que hace la [Parte 2](../Practica-2-Temporizacion-No-Bloqueante-Parte-2-millis) con `millis()`.

## Resultados
[Resultados de la práctica — Temporización con `delay()` (PDF)](Resultados/Resultados-Temporizacion-delay.pdf) · [qué contiene la carpeta](Resultados/Readme.txt)

Resultados obtenidos en la práctica:

- Los LEDs parpadean, pero cada uno enciende una sola vez por ciclo y el ciclo dura 6 s
- En 6 s se esperaban 6 / 3 / 2 encendidos y se obtuvo 1 / 1 / 1
- Nunca hay dos LEDs encendidos al mismo tiempo: es el antipatrón visto con los propios ojos
