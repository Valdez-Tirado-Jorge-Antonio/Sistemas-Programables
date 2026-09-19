Resultados y evidencias de la practica.

  Resultados-Buffer-Circular-ISR.pdf  - Resultados obtenidos, tabla de pruebas, tabla de
                                        intervalos, captura del Monitor Serie y evidencias   [LISTO]

Pruebas documentadas:
  1. Pulsaciones lentas: cada una sale como "Pieza N", sin saltos.
  2. Pausas largas (hasta 31 s): ninguna pieza fantasma.
  3. Rafaga rapida (7 pulsaciones en 1.4 s): la cuenta no se salta ninguna; intervalos de 200-300 ms.
  4. Llenar el buffer: no se logro a mano, el loop() lo vacia en microsegundos (perdidas = 0).
  5. La animacion de la matriz no se detuvo en ninguna prueba.
