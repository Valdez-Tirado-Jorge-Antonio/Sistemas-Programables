# Guía de hardware — armado, conexión y pruebas

Complemento del [Readme.md](Readme.md) de la práctica: cubre el armado físico, la
conexión a la red y la solución de problemas. Sigue esto en orden.

---

## Antes de salir de casa

Checa que llevas:

- [ ] Arduino UNO R4 WiFi + cable USB-C
- [ ] Laptop con el IDE de Arduino
- [ ] Celular Android con MIT AI2 Companion
- [ ] Motorreductor
- [ ] Puente H L298N
- [ ] Fuente externa para el motor (pila 9V, portapilas de 6 AA, o eliminador)
- [ ] Cables dupont macho-hembra
- [ ] El nombre y contraseña del WiFi que vas a usar

> **Importante**: la red debe ser de **2.4 GHz**. El R4 no ve las de 5 GHz.
> Si en la escuela el WiFi es institucional y pide usuario y contraseña
> (tipo WPA2-Enterprise), **no va a funcionar**. Plan B: prende el hotspot
> de tu celular en 2.4 GHz y conecta el Arduino ahí. Como el celular es el
> mismo que corre la app, quedan en la misma red automáticamente.

---

## Armado y puesta en marcha

### 1. Cablear

**Con todo desconectado de la corriente.**

| L298N | va a |
|---|---|
| ENA | Arduino pin **9** |
| IN1 | Arduino pin **8** |
| IN2 | Arduino pin **7** |
| GND | Arduino **GND** |
| 12V | positivo de la fuente externa |
| GND | negativo de la fuente externa |
| OUT1 | una terminal del motor |
| OUT2 | la otra terminal del motor |

Dos cosas que se olvidan y arruinan todo:

1. **El GND del Arduino y el GND de la fuente deben estar unidos.**
   Sin eso el L298N no entiende las señales y el motor no se mueve.
2. **Quita el jumper del ENA** si tu L298N lo trae puesto. Ese jumper
   fuerza la velocidad al máximo e ignora tu PWM.

### 2. Subir el código

1. Abre `Codigo/ControlMotor.ino`
2. Pon tu SSID y PASS en las dos líneas de arriba del archivo:
   ```cpp
   const char* SSID = "MiWiFi";
   const char* PASS = "micontraseña";
   ```
3. Herramientas → Placa → **Arduino UNO R4 WiFi**
   (si no aparece: Gestor de tarjetas → busca "Arduino UNO R4" → instalar)
4. Herramientas → Puerto → el que diga UNO R4
5. Sube el código (flecha →)
6. Abre el **Monitor Serie** a **9600** y anota la IP que imprime:
   ```
   Conectado
   >>> COPIA ESTA IP A LA APP:  http://192.168.1.73
   ```
   Anótala completa, con el `http://` y sin barra al final.

### 3. Ajustar la app

Proyecto **ControlMotor**, Blocks:

- `initialize global ip` → la IP nueva
- `initialize global conectado` → **true**

### 4. Probar

| Di esto | Nivel | Debe pasar |
|---|---|---|
| "aumentar" | 1 | el motor arranca lento |
| "aumentar" | 2 | acelera |
| "aumentar" | 3 | va al máximo |
| "aumentar" | 3 | te avisa que ya está al tope, no cambia |
| "retroceder" | 2 | baja un escalón |
| "alto" | 0 | se detiene de golpe |
| "retroceder" | 0 | te avisa que ya está detenido |

---

## Si algo falla

### El Arduino no se conecta al WiFi

| Causa | Solución |
|---|---|
| Red de 5 GHz | Usa la de 2.4, o el hotspot del celular |
| WiFi con usuario y contraseña | No sirve. Usa hotspot |
| SSID mal escrito | Distingue mayúsculas. Cópialo tal cual |
| Muy lejos del router | Acércate |

### Se conecta pero la app no le pega

1. **¿Están en la misma red?** El celular no puede estar en datos móviles.
2. **¿Pusiste el `http://`?** Sin eso el componente Web no funciona.
3. **¿Dejaste barra al final?** `http://192.168.1.73/` está mal, quita la barra.
4. **¿`conectado` quedó en `true`?** Es el error más común.
5. Prueba abriendo `http://LA_IP/led/on` en el navegador de la laptop.
   Si desde ahí sí prende, el problema está en la app. Si no prende,
   está en la red o en el Arduino.

### La IP cambió al día siguiente

Es normal, el router la reasigna. Vuelve a abrir el Monitor Serie y anótala.

Para evitarlo, agrega esto en `setup()` **antes** del `WiFi.begin`:

```cpp
IPAddress ipFija(192, 168, 1, 200);
IPAddress puerta(192, 168, 1, 1);
IPAddress mascara(255, 255, 255, 0);
WiFi.config(ipFija, puerta, mascara);
```

Cambia los números según tu red (mira los primeros tres números
de la IP que te dio antes).

### El motor no gira

> **Empieza por medir la batería.** Una LiPo baja pierde voltaje y sube su resistencia
> interna, así que el motor arranca en el nivel 3 pero solo zumba en el 1. Parece un
> problema de calibración del PWM y no lo es. A nosotros nos pasó y perdimos tiempo
> ajustando el código antes de darnos cuenta.

| Síntoma | Causa probable |
|---|---|
| No se mueve en ningún nivel | Falta unir los GND, o falta la fuente externa |
| Solo gira a full | El jumper del ENA está puesto |
| Zumba pero no arranca en "baja" | **Batería descargada.** Es la causa más común y la que nos costó encontrar. Mídela y cárgala antes de tocar el código |
| El Arduino se reinicia al arrancar | Estás alimentando el motor desde el Arduino. Usa fuente aparte |

### La app dice "No te entendí"

Mira `LblComando`: ahí sale el texto exacto que escuchó Google.
Copia ese texto al `piece` del `contains` que corresponda.

Truco: usa pedazos cortos. `"aumenta"` atrapa más variantes que
`"aumentar"`, y `"retroced"` atrapa tanto "retrocede" como "retroceder".

---

## Para la demostración

Si el WiFi falla el día de la entrega, **la app sigue siendo demostrable**:
pon `conectado` en `false` y toda la lógica de voz, las respuestas habladas y
los tres niveles de velocidad funcionan igual, solo sin mover el hardware.

Vale la pena llevar las dos versiones listas por si acaso.
