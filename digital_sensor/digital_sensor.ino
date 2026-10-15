/*
 * Kiwistron
 * KiwisIoT (Web-based IoT platform for sending & receiving data)
 * Author: Varun Prasad
 */
#include <KiwisIoT.h>   // Library for KiwisIoT communication

 char* ssid  = "YOUR_WIFI_SSID";        
 char* pass  = "YOUR_WIFI_PASSWORD";   
 char* topic = "YOUR_DASHBOARD_TOPIC"; // Dashboard topic ID

KiwisIoT kiwisiot(ssid, pass, topic); // IoT object

#define SENSOR_PIN D5  

int state = 0;          

void setup() {
  Serial.begin(9600);        
  pinMode(SENSOR_PIN, INPUT);
  kiwisiot.begin();          // Start IoT connection
}

void loop() {
  kiwisiot.run();            // Maintain connection

  state = digitalRead(SENSOR_PIN);

  kiwisiot.send("1", state); // Send to widget ID "1"

  Serial.print("Digital State = ");
  Serial.println(state);    

  delay(1000); 
}
