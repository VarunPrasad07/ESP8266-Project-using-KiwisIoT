#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <DHT.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// MAX Sensor
MAX30105 particleSensor;

// DHT
#define DHT_PIN D4
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// ALERT
#define LED_PIN D6
#define BUZZER_PIN D7

unsigned long lastSend = 0;
const long interval = 3000;

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin();

  // MAX sensor start
  if (!particleSensor.begin(Wire))
  {
    Serial.println("MAX sensor not found");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);

  dht.begin();

  kiwisiot.begin();

  Serial.println("Patient Monitoring System Started");
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    // ----- Heart rate reading -----
    long irValue = particleSensor.getIR();

    int heartRate = random(70,95);  // demo heart rate
    int spo2 = random(95,100);      // demo spo2

    // ----- DHT reading -----
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    // ----- Send data -----
    kiwisiot.send("0", heartRate);
    kiwisiot.send("1", spo2);

    if(!isnan(temp) && !isnan(hum))
    {
      kiwisiot.send("2", temp);
      kiwisiot.send("3", hum);
    }

    // ----- Alert condition -----
    if(heartRate > 100 || heartRate < 60)
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);

      kiwisiot.send("4",1);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      kiwisiot.send("4",0);
    }

    Serial.print("HR: ");
    Serial.print(heartRate);
    Serial.print("  SpO2: ");
    Serial.print(spo2);
    Serial.print("  Temp: ");
    Serial.println(temp);
  }
}
