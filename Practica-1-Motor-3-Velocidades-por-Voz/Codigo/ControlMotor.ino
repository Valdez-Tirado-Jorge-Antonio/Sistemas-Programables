/*
  Practica 1 - Velocidad del motorreductor por comando de voz
  Placa: Arduino UNO R4 WiFi
  Driver: puente H L298N

  Rutas que recibe (las manda App Inventor):
    /vel/0   detenido
    /vel/1   velocidad baja
    /vel/2   velocidad media
    /vel/3   velocidad maxima

  Devuelve el nombre de la velocidad en texto plano, y la app
  lo lee en voz alta.

  CONEXIONES
    L298N  ->  Arduino
      ENA  ->  pin 9   (tiene que ser PWM, quitar el jumper de ENA)
      IN1  ->  pin 8
      IN2  ->  pin 7
      GND  ->  GND     (obligatorio, GND comun)
    L298N  ->  bateria LiPo 11.1V
      12V  ->  positivo
      GND  ->  negativo
    L298N  ->  motor
      OUT1 y OUT2  ->  las dos terminales del motorreductor

  NOTAS DE CALIBRACION
    La patada de arranque (255 durante ARRANQUE_MS y luego el valor
    del nivel) se agrego cuando el motor no arrancaba en el nivel 1.

    OJO: la causa real de eso NO era el motor, era la bateria
    descargada. Con la bateria cargada arranca directo con PWM 130.
    La patada se dejo porque da margen si la bateria va a media
    carga, pero no es la solucion al problema.

    Si el motor no arranca: mide primero el voltaje de la bateria.
    Solo despues de descartarla, ajusta PWM[] y ARRANQUE_MS.
*/

#include <WiFiS3.h>

// ---------- CAMBIA ESTO ----------
const char* SSID = "NOMBRE_DE_TU_WIFI";
const char* PASS = "TU_PASSWORD";
// ---------------------------------

const int ENA = 9;
const int IN1 = 8;
const int IN2 = 7;

const int ARRANQUE_MS = 150;   // duracion de la patada de arranque

WiFiServer servidor(80);

int nivel = 0;   // 0 = detenido, 1 = baja, 2 = media, 3 = maxima

const int PWM[4] = { 0, 130, 190, 255 };

const char* NOMBRE[4] = {
  "detenido",
  "velocidad baja",
  "velocidad media",
  "velocidad maxima"
};

void aplicarVelocidad() {
  if (nivel == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENA, 255);        // patada de arranque
    delay(ARRANQUE_MS);
    analogWrite(ENA, PWM[nivel]); // velocidad real
  }

  Serial.print("Nivel ");
  Serial.print(nivel);
  Serial.print("  PWM ");
  Serial.println(PWM[nivel]);
}

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  aplicarVelocidad();

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("NO SE PUDO CONECTAR");
    Serial.println("Revisa: nombre, password, y que la red sea de 2.4 GHz");
    return;
  }

  Serial.println();
  Serial.println("Conectado");
  Serial.print(">>> COPIA ESTA IP A LA APP:  http://");
  Serial.println(WiFi.localIP());

  servidor.begin();
}

void loop() {
  WiFiClient cliente = servidor.available();
  if (!cliente) return;

  String peticion = cliente.readStringUntil('\n');
  while (cliente.available()) cliente.read();

  Serial.print("Recibi: ");
  Serial.println(peticion);

  int pos = peticion.indexOf("/vel/");
  if (pos >= 0) {
    char c = peticion.charAt(pos + 5);
    if (c >= '0' && c <= '3') {
      nivel = c - '0';
      aplicarVelocidad();
    }
  }

  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-Type: text/plain");
  cliente.println("Connection: close");
  cliente.println();
  cliente.println(NOMBRE[nivel]);

  delay(5);
  cliente.stop();
}
