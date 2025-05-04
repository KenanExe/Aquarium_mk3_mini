#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"

const int B222 = 35; //its degres buttons  set auto temp to 22.2
const int B22 = 34; //its degres buttons  set auto temp to 22.0
const int B128 = 39; //its degres buttons  set auto temp to -128 (maunel on the fan)
float ButtonSev = 22.5; //its degres buttons  set auto temp to  22.5 (stock degree)

#define sleepTime 12e7 // (120,000,000 micro seconts = 2minute)

#define LED 2
#define ONE_WIRE_BUS 32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

#define relay 33 // relay (fan)

void setup() {
  pinMode(LED,OUTPUT);
  pinMode(B22,INPUT);
  pinMode(B222,INPUT);
  pinMode(B128,INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) {
    Serial.println("Unable to find address for Device 0");
    return;
  }

  sensors.setResolution(insideThermometer, 11);
  pinMode(relay, OUTPUT);
}

#define LED 2  // ESP32 blue led

void wifi() {
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);

  WiFi.disconnect(true);  
  delay(100);             

  WiFi.begin(ssid, pass);  // Restart the wifi
  WiFi.setAutoReconnect(true); // Auto connection on

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, HIGH);
    delay(200);               
    digitalWrite(LED, LOW);
    delay(200);               
    Serial.print(".");

    if (millis() - startTime > 8000) {  // 8 seconds to restart
      Serial.println("\nConnotConnected");
      ESP.restart();
    }
  }

  Serial.println("\nConnected");
  digitalWrite(LED, LOW);
}


void Butons() {
  ButtonSev = 22.5;

  int buttonState1 = digitalRead(B22);
  if (buttonState1 == HIGH) {
    ButtonSev = 22.0;
  }

  int buttonState2 = digitalRead(B222);
  if (buttonState2 == HIGH) {
    ButtonSev = 22.2;
  }

  int buttonState3 = digitalRead(B128);
  if (buttonState3 == HIGH) {
    ButtonSev = -128.0;
  }

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifi();
  }
  
  Butons();

  digitalWrite(LED,HIGH);
  sensors.requestTemperatures();
  float temperature = sensors.getTempC(insideThermometer);

  if (temperature >= ButtonSev) {
    digitalWrite(relay, HIGH);
    ThingSpeak.setField(3, 1);
  } else {
      digitalWrite(relay, LOW);
      ThingSpeak.setField(3, 0);
  }

  if (temperature == -127) { //ESP sensonrs fuse
    sensors.requestTemperatures();
    float temperature = sensors.getTempC(insideThermometer);
    if (temperature == -127) {
            ESP.restart(); 
    }
  }
    ThingSpeak.setField(2, temperature);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  digitalWrite(LED,LOW);
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}
