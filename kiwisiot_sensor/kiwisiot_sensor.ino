#include <KiwisIoT.h>

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

#define SENSOR_PIN A0   // Analog sensor pin (ESP32)

KiwisIoT kiwisiot(ssid, pass, topic);

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  kiwisiot.begin();
}

void loop() {
  kiwisiot.run();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 2000) {
    lastSend = millis();

    int sensorValue = analogRead(SENSOR_PIN);
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);

    kiwisiot.send("0", sensorValue);  
  }
}
