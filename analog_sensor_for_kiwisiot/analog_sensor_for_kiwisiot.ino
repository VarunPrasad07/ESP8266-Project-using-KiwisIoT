/*
 * Kiwistrons
 * KiwisIoT (Web-based IoT platform for sending & receiving data)
 * Author: Varun Prasad
 */

#include <KiwisIoT.h>   // Library for KiwisIoT communication

 char* ssid  = "YOUR_WIFI_SSID";        // WiFi name
 char* pass  = "YOUR_WIFI_PASSWORD";   // WiFi password
 char* topic = "YOUR_DASHBOARD_TOPIC"; // Dashboard topic

KiwisIoT kiwisiot(ssid, pass, topic); // IoT object

#define SENSOR_PIN A0   

int value = 0;          

void setup() {
  Serial.begin(9600);   
  kiwisiot.begin();     // Start IoT connection
}

void loop() {
  kiwisiot.run();       // Maintain connection

  value = analogRead(SENSOR_PIN); 

  kiwisiot.send("1", value); // Send to widget ID "1"

  Serial.print("Analog Value = ");
  Serial.println(value);

  delay(1000);
}
