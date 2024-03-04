/* Se usara la placa esp32 8266-01s
  
  IMPORTANTE: Hay que comentar en  
  que es la parte ambigua de recibir un float o un int, por que si no se rompe
  
  IMPORTANTE: Por alguna razon el platformio nuevo fuerza la instalacion de la ArduinoJson
  en la version mas reciente 6.21.2 y esta trae conflictos con la thingsboard.h por
  eso se comentan las lineas de codigo. Si no queres comentar las lineas de codigo,
  borra del libdeps el ArduinoJson 6.20.0 e instalalo directamente desde el pio home.

  IMPORTANTE: Si esta activado el traductor automatico y queres ver los datos en thingsboard
  desde el ultima telimetria, se traba.
  
  Utilizar las siguientes versiones de librerias:
  lib_deps = 
	    powerbroker2/SafeString@^4.1.27
	    paulstoffregen/OneWire@^2.3.7
	    milesburton/DallasTemperature@^3.11.0
	    ESP8266WiFi @ ^1.0
	    thingsboard/TBPubSubClient@^2.9.1
	    bblanchon/ArduinoJson @ ^6.20.0
	    thingsboard/ThingsBoard@0.6
	    knolleary/PubSubClient@^2.8

  OBS: En el programador se cortocircuita(se pone un valor bajo) IO0 con GND
 */

#include <Arduino.h>
#include <ThingSpeak.h>
#include <ESP8266Wifi.h>
#include <ThingsBoard.h>

#define SSID "Edificio INNOVA"
#define PASS  "Mexico932"
#define TOKEN "akiot_prueba"
#define THINGSBOARD_SERVER  "panel.akiot.es"
#define TEMP_MAX 23.5
#define TEMP_MIN 22
#define RANGO 1

unsigned long lastSend;
unsigned long channelID = 2277765;
const char* writeAPIKey = "LX9M0W8D4FOMK1R4";
float temperatura;

WiFiClient espClient;
ThingsBoard tb(espClient);

void wifi(); 

void setup() {
  Serial.begin(9600);
  wifi();
  ThingSpeak.begin(espClient);
}

void loop() {
  if ( millis() - lastSend > 10000 ) { 
    if (WiFi.status() != WL_CONNECTED) {
      wifi();
      }
      if (!tb.connected()) {
      Serial.print("Connecting to: ");
      Serial.print(THINGSBOARD_SERVER);
      Serial.print(" with token ");
      Serial.println(TOKEN);
        if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
        Serial.println("Failed to connect");
        }
      }
    lastSend = millis();
  }

  tb.sendTelemetryFloat("temp_thingspeak",temperatura);
  temperatura = random(20,30);
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  ThingSpeak.setField(1,temperatura);
  ThingSpeak.writeFields(channelID,writeAPIKey);
  delay(20000);
}

void wifi(){

/* Esta funcion de WiFi prueba conexion durante 10 intentos
si pasado estos intentos no se conecta sigue, esto es una
funcion No bloqueante a medias (5seg), lo bueno que si
se cae la red no queda en un loop infinito y los equipos
pueden seguir controlando normalmente. */

  int intentos = 0;
  WiFi.begin(SSID,PASS);
  while (WiFi.status() != WL_CONNECTED && intentos < 10){
    delay(500);
    intentos++;
  }

}