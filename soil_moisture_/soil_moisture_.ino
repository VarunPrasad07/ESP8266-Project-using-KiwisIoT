
#include <KiwisIoT.h>

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";  // Dashboard topic ID

KiwisIoT kiwisiot(ssid, pass, topic);

#define SENSOR_PIN A0

// Soil moisture sensor calibration values
#define DRY_VALUE 1023
#define WET_VALUE 300

int rawValue = 0;
int moisture = 0;

void setup() {
  Serial.begin(9600);

  kiwisiot.begin();  // Start IoT connection
}

void loop() {
  kiwisiot.run();  // Maintain connection

  // Read soil moisture sensor
  rawValue = analogRead(SENSOR_PIN);

  // Convert sensor value to 0-100%
  moisture = map(rawValue, DRY_VALUE, WET_VALUE, 0, 100);

  // Limit the value between 0 and 100
  moisture = constrain(moisture, 0, 100);

  // Send moisture percentage to KiwisIoT
  kiwisiot.send("1", moisture);

  // Display values in Serial Monitor
  Serial.print("Raw Sensor Value = ");
  Serial.print(rawValue);

  Serial.print(" | Soil Moisture = ");
  Serial.print(moisture);
  Serial.println("%");

  delay(1000);
}
