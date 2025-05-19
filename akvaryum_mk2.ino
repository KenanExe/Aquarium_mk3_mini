#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"

bool Sleepy = false;

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

    if (millis() - startTime > 8000) {  // 8 seconds to restart (fuse)
      Serial.println("\nConnotConnected");
      ESP.restart();
    }
  }

  Serial.println("\nConnected");
  digitalWrite(LED, LOW);
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifi();
  }
  
  
  digitalWrite(LED,HIGH);
  sensors.requestTemperatures();
  float temperature = sensors.getTempC(insideThermometer);

  if (temperature >= 22.5) {
    digitalWrite(relay, HIGH);
    ThingSpeak.setField(3, 1);
    Sleepy = false;
  } else {
      digitalWrite(relay, LOW);
      ThingSpeak.setField(3, 0);
      Sleepy = true;
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

  if (Sleepy == false) {
    delay(120000);
  } else {
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
  }
}
