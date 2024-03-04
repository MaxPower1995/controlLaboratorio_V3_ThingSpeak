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
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SSID "Edificio INNOVA"
#define PASS  "Mexico932"
#define TEMP_MAX 23.5
#define TEMP_MIN 22
#define RANGO 1

unsigned long lastSend;
unsigned long channelID = 2277765;
const char* writeAPIKey = "LX9M0W8D4FOMK1R4";
const char* readAPIKey = "JSLLDU4BJV6ZW6NR";
float temperatura;
float estado = 0;

const char* ntpServer = "pool.ntp.org";
const int timeZone = -3;
int hora;
int dia;

WiFiClient espClient;
ThingsBoard tb(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, timeZone * 3600);

void wifi(); 
void controlTemperaturaFrio(float* temperatura);

void setup() {
  Serial.begin(9600);
  wifi();
  ThingSpeak.begin(espClient);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);
  digitalWrite(0,HIGH);
  timeClient.begin();
  timeClient.update();
}

void loop() {

  if ( millis() - lastSend > 10000 ) { 
    if (WiFi.status() != WL_CONNECTED) {
      wifi();
      }

    timeClient.begin();
    timeClient.update();
    hora = timeClient.getHours();
    dia = timeClient.getDay();
    lastSend = millis();

    temperatura = ThingSpeak.readFloatField(channelID,1,readAPIKey);

    Serial.print("Temperatura: ");
    Serial.println(temperatura);
    Serial.print("Estado: ");
    Serial.println(estado);
  }

    if (hora < 17 && hora >= 7 && dia < 6 && dia > 0) {
      controlTemperaturaFrio(&temperatura);
    }else{
      digitalWrite(2,HIGH);
      digitalWrite(0,HIGH);
    }
    
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

void controlTemperaturaFrio(float* temperatura){
  if(*temperatura > TEMP_MAX){ //if(*temperatura > TEMP_MAX){
    digitalWrite(2,LOW);
    digitalWrite(0,LOW);
    estado = 1;
  }

  if (*temperatura < (TEMP_MAX - RANGO)){ //if (*temperatura < (TEMP_MAX - RANGO))
    digitalWrite(2,HIGH);
    digitalWrite(0,HIGH);
    estado = 0;
  }
  
}