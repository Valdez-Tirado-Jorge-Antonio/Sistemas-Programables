Capturas del Monitor Serie (9600 baudios).

  monitor-serie-piezas-47-80.png  - Parte final de una sesion de 80 pulsaciones: lineas
                                    "Pieza N - t = ... ms" con el millis() que anoto la ISR   [LISTO]

Que se ve en la captura:
  - Pulsaciones lentas (47-51, 52-54): una linea por pulsacion, numeracion de uno en uno
  - Pausas largas (6 s, 31 s, 27 s): ninguna pieza fantasma
  - Rafaga media (61-73): 13 pulsaciones en 5.7 s, intervalos de 300-700 ms
  - Rafaga rapida (74-80): 7 pulsaciones en 1.4 s, intervalos de 200-300 ms, sin huecos
